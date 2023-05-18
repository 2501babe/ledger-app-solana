#pragma once

#include "sol/parser.h"
#include "sol/printer.h"

extern const Pubkey compute_budget_program_id;

enum ComputeBudgetInstructionKind {
    ComputeBudgetRequestUnitsDeprecated,
    ComputeBudgetRequestHeapFrame,
    ComputeBudgetSetComputeUnitLimit,
    ComputeBudgetSetComputeUnitPrice,
};

typedef struct ComputeBudgetRequestHeapFrameInfo {
    uint64_t bytes;
} ComputeBudgetRequestHeapFrameInfo;

typedef struct ComputeBudgetSetComputeUnitLimitInfo {
    uint64_t units;
} ComputeBudgetSetComputeUnitLimitInfo;

typedef struct ComputeBudgetSetComputeUnitPriceInfo {
    uint64_t micro_lamports;
} ComputeBudgetSetComputeUnitPriceInfo;

typedef struct ComputeBudgetInfo {
    enum ComputeBudgetInstructionKind kind;
    union {
        ComputeBudgetRequestHeapFrameInfo request_heap_frame;
        ComputeBudgetSetComputeUnitLimitInfo set_compute_unit_limit;
        ComputeBudgetSetComputeUnitPriceInfo set_compute_unit_price;
    };
} ComputeBudgetInfo;

int parse_compute_budget_instructions(const Instruction* instruction,
                                      const MessageHeader* header,
                                      ComputeBudgetInfo* info);
int print_compute_budget_info(const ComputeBudgetInfo* info, const PrintConfig* print_config);

// XXX the stake header has a handful of other print functions, check why theyre exported. tests maybe
