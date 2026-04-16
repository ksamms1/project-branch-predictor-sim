# plot_experiments.R
# Read simulator CSV outputs, compute degradation, and generate clearer plots
# for single-program baselines vs multi-program shared-predictor experiments.

if (!requireNamespace("ggplot2", quietly = TRUE)) {
  stop("Package 'ggplot2' is not installed. Install it with: sudo apt install r-cran-ggplot2")
}

library(ggplot2)

# ---- paths ----
single_file <- "results/single_program_baselines.csv"
multi_file  <- "results/multi_program_results.csv"
derived_file <- "results/degradation_comparison.csv"

plots_dir <- "results/plots"
if (!dir.exists(plots_dir)) {
  dir.create(plots_dir, recursive = TRUE)
}

# ---- read data ----
single_df <- read.csv(single_file, stringsAsFactors = FALSE)
multi_df  <- read.csv(multi_file, stringsAsFactors = FALSE)

# ---- basic type cleanup ----
single_df$misprediction_rate <- as.numeric(single_df$misprediction_rate)

multi_df$slice <- as.numeric(multi_df$slice)
multi_df$program_count <- as.integer(multi_df$program_count)
multi_df$program_index <- as.integer(multi_df$program_index)
multi_df$branches_loaded <- as.numeric(multi_df$branches_loaded)
multi_df$branches_executed <- as.numeric(multi_df$branches_executed)
multi_df$predictions <- as.numeric(multi_df$predictions)
multi_df$mispredictions <- as.numeric(multi_df$mispredictions)
multi_df$misprediction_rate <- as.numeric(multi_df$misprediction_rate)
multi_df$total_predictions <- as.numeric(multi_df$total_predictions)
multi_df$total_mispredictions <- as.numeric(multi_df$total_mispredictions)
multi_df$total_misprediction_rate <- as.numeric(multi_df$total_misprediction_rate)
multi_df$scheduler_rounds <- as.numeric(multi_df$scheduler_rounds)

# ---- normalize single baselines so they can be plotted alongside multi results ----
single_plot_df <- data.frame(
  predictor = single_df$predictor,
  predictor_args = single_df$predictor_args,
  trace_set = single_df$trace_name,
  slice = NA,
  misprediction_rate = single_df$misprediction_rate,
  stringsAsFactors = FALSE
)

# ---- summarize multi results to ONE row per run ----
# For 2-trace and 3-trace runs, use total misprediction rate as the trace-set metric.
# Since total_misprediction_rate is repeated on each row of a run, keep one copy.
multi_summary_df <- unique(
  multi_df[, c(
    "run_id",
    "predictor",
    "predictor_args",
    "slice",
    "program_count",
    "trace_set",
    "total_predictions",
    "total_mispredictions",
    "total_misprediction_rate",
    "scheduler_rounds"
  )]
)

names(multi_summary_df)[names(multi_summary_df) == "total_misprediction_rate"] <- "misprediction_rate"

# ---- compute degradation against single baselines ----
# For multi-trace sets, compare against the average of the constituent single-trace baselines.
single_baseline_lookup <- single_df[, c("predictor", "predictor_args", "trace_name", "misprediction_rate")]
names(single_baseline_lookup)[names(single_baseline_lookup) == "misprediction_rate"] <- "single_misprediction_rate"

compute_trace_set_baseline <- function(predictor, predictor_args, trace_set, lookup_df) {
  traces <- unlist(strsplit(trace_set, "\\+"))
  rows <- lookup_df[
    lookup_df$predictor == predictor &
      lookup_df$predictor_args == predictor_args &
      lookup_df$trace_name %in% traces,
  ]

  if (nrow(rows) != length(traces)) {
    return(NA_real_)
  }

  mean(rows$single_misprediction_rate)
}

multi_summary_df$single_baseline_rate <- mapply(
  compute_trace_set_baseline,
  multi_summary_df$predictor,
  multi_summary_df$predictor_args,
  multi_summary_df$trace_set,
  MoreArgs = list(lookup_df = single_baseline_lookup)
)

multi_summary_df$absolute_degradation <-
  multi_summary_df$misprediction_rate - multi_summary_df$single_baseline_rate

multi_summary_df$relative_degradation_percent <-
  ifelse(
    is.na(multi_summary_df$single_baseline_rate) | multi_summary_df$single_baseline_rate == 0,
    NA,
    100 * multi_summary_df$absolute_degradation / multi_summary_df$single_baseline_rate
  )

# ---- write derived CSV ----
write.csv(
  multi_summary_df[, c(
    "predictor",
    "predictor_args",
    "slice",
    "program_count",
    "trace_set",
    "misprediction_rate",
    "single_baseline_rate",
    "absolute_degradation",
    "relative_degradation_percent",
    "total_predictions",
    "total_mispredictions",
    "scheduler_rounds"
  )],
  derived_file,
  row.names = FALSE
)

# ---- plot 1: multi-program trace-set misprediction rate vs slice ----
# Discrete tested slice values: use points, with thin lines only as visual guides.
p1 <- ggplot(
  multi_summary_df,
  aes(
    x = slice,
    y = misprediction_rate,
    color = trace_set,
    group = trace_set
  )
) +
  geom_point(size = 2.5) +
  geom_line(size = 0.5, alpha = 0.6) +
  facet_wrap(~ predictor, scales = "free_y") +
  labs(
    title = "Multi-program trace-set misprediction rate vs. time slice",
    x = "Time slice (tested values only)",
    y = "Misprediction rate (%)",
    color = "Trace set"
  ) +
  theme_minimal()

ggsave(
  filename = file.path(plots_dir, "trace_set_rate_vs_slice.png"),
  plot = p1,
  width = 11,
  height = 7,
  dpi = 300
)

# ---- plot 2: degradation vs slice ----
p2 <- ggplot(
  multi_summary_df,
  aes(
    x = slice,
    y = absolute_degradation,
    color = trace_set,
    group = trace_set
  )
) +
  geom_point(size = 2.5) +
  geom_line(size = 0.5, alpha = 0.6) +
  facet_wrap(~ predictor, scales = "free_y") +
  labs(
    title = "Trace-set degradation vs. time slice",
    x = "Time slice (tested values only)",
    y = "Absolute degradation from single-trace baseline (%)",
    color = "Trace set"
  ) +
  theme_minimal()

ggsave(
  filename = file.path(plots_dir, "trace_set_degradation_vs_slice.png"),
  plot = p2,
  width = 11,
  height = 7,
  dpi = 300
)

# ---- plot 3: degradation heatmap ----
# One tile per tested slice and trace set.
# Fill color shows absolute degradation from the single-trace baseline.

p3 <- ggplot(
  multi_summary_df,
  aes(
    x = factor(slice),
    y = trace_set,
    fill = absolute_degradation
  )
) +
  geom_tile(color = "white") +
  facet_wrap(~ predictor, scales = "free_y") +
  scale_fill_gradient2(
    low = "blue",
    mid = "white",
    high = "red",
    midpoint = 0,
    name = "Degradation (%)"
  ) +
  labs(
    title = "Heatmap of degradation by trace set and time slice",
    x = "Time slice (tested values only)",
    y = "Trace set"
  ) +
  theme_minimal()

ggsave(
  filename = file.path(plots_dir, "degradation_heatmap.png"),
  plot = p3,
  width = 11,
  height = 7,
  dpi = 300
)

# ---- plot 4: single-program baseline reference chart ----
p4 <- ggplot(
  single_df,
  aes(
    x = trace_name,
    y = misprediction_rate,
    color = trace_name
  )
) +
  geom_point(size = 3) +
  facet_wrap(~ predictor, scales = "free_y") +
  labs(
    title = "Single-program baseline misprediction rates",
    x = "Trace",
    y = "Misprediction rate (%)",
    color = "Trace"
  ) +
  theme_minimal()

ggsave(
  filename = file.path(plots_dir, "single_program_baselines.png"),
  plot = p4,
  width = 10,
  height = 6,
  dpi = 300
)

cat("Wrote derived CSV to:", derived_file, "\n")
cat("Wrote plots to:", plots_dir, "\n")