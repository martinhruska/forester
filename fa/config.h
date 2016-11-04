/*
 * Copyright (C) 2012 Jiri Simacek
 *
 * This file is part of forester.
 *
 * forester is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * forester is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with forester.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file config.h
 * various compile-time options
 */

#ifndef CONFIG_H
#define CONFIG_H

/**
 * set abstraction height (default is 1)
 */
#define FA_ABS_HEIGHT                   1

/**
 * set reference count tracking treshold (default is 2)
 */
#define FA_REF_CNT_TRESHOLD             2

/**
 * allow to track number of selectors leading towards a given cut-point (default is 1)
 */
#define FA_TRACK_SELECTORS              1

/**
 * allow folding of nested structures (default is 1)
 */
#define FA_ALLOW_FOLDING                1

/**
 * overapproximate when folding (default is 1)
 */
#define FA_BOX_APPROXIMATION            1

/**
 * should we restart every time a new box is encountered (default is 1)
 */
#define FA_RESTART_AFTER_BOX_DISCOVERY  (1 + FA_BOX_APPROXIMATION)

/**
 * enable fusion when computing abstraction (default is 1)
 */
#define FA_FUSION_ENABLED					       1

/**
 * use predicate abstraction (default abstraction is finite height abstraction)
 */
#define FA_USE_PREDICATE_ABSTRACTION     1

/**
 * start with predicate abstraction or height abstraction
 */
#define FA_START_WITH_PREDICATE_ABSTRACTION     0

/**
 * allow to use structures with zero size, e.g. pointers to undefined types
 */
#define FA_ALLOW_ZERO_STRUCT        1

/**
 * forester will continue in analysis also after detection of a garbage
 */
#define FA_CONTINUE_WITH_GARBAGE    1

#define FA_ALLOW_STACK_FRAME_ABSTRACTION 1

/**
 * detect spurious counterexamples using backward run
 */
#define FA_BACKWARD_RUN 1

// Cut spurious parts of split -> unsound
#define SPURIOUS_CUT 0

// Finish all paths in symbolic execution
#define FINISHING_SYMEXEC 0

// Abstraction ignores leave of predicates and abstracted TA
#define ABSTR_IGNORE_LEAVES 0

#endif /* CONFIG_H */
