#include "common_byte_strings.h"
#include "instruction.h"
#include "sol/parser.h"
#include "sol/transaction_summary.h"
#include "compute_budget_instruction.h"
#include "util.h"
#include <stdbool.h>
#include <string.h>

const Pubkey compute_budget_program_id = {{PROGRAM_ID_COMPUTE_BUDGET}};

static int parse_compute_budget_instruction_kind(Parser* parser, enum ComputeBudgetInstructionKind* kind) {
    uint32_t maybe_kind;
    BAIL_IF(parse_u32(parser, &maybe_kind));
    switch (maybe_kind) {
        case ComputeBudgetRequestUnitsDeprecated:
        case ComputeBudgetRequestHeapFrame:
        case ComputeBudgetSetComputeUnitLimit:
        case ComputeBudgetSetComputeUnitPrice:
            *kind = (enum ComputeBudgetInstructionKind) maybe_kind;
            return 0;
    }
    return 1;
}

// XXX these three parse functions are boilerplate and could be axed for a more succinct style if desired

static int parse_compute_budget_heap_frame_instruction(Parser* parser,
                                                       const Instruction* instruction,
                                                       const MessageHeader* header,
                                                       ComputeBudgetRequestHeapFrameInfo* info) {
    BAIL_IF(parse_u64(parser, &info->bytes));

    return 0;
}

static int parse_compute_budget_unit_limit_instruction(Parser* parser,
                                                       const Instruction* instruction,
                                                       const MessageHeader* header,
                                                       ComputeBudgetSetComputeUnitLimitInfo* info) {
    BAIL_IF(parse_u64(parser, &info->units));

    return 0;
}

static int parse_compute_budget_unit_price_instruction(Parser* parser,
                                                       const Instruction* instruction,
                                                       const MessageHeader* header,
                                                       ComputeBudgetSetComputeUnitPriceInfo* info) {
    BAIL_IF(parse_u64(parser, &info->micro_lamports));

    return 0;
}

int parse_compute_budget_instructions(const Instruction* instruction,
                                      const MessageHeader* header,
                                      ComputeBudgetInfo* info) {
    Parser parser = {instruction->data, instruction->data_length};

    BAIL_IF(parse_compute_budget_instruction_kind(&parser, &info->kind));

    switch (info->kind) {
        case ComputeBudgetRequestHeapFrame:
            return parse_compute_budget_heap_frame_instruction(&parser, instruction, header, &info->request_heap_frame);
        case ComputeBudgetSetComputeUnitLimit:
            return parse_compute_budget_unit_limit_instruction(&parser, instruction, header, &info->set_compute_unit_limit);
        case ComputeBudgetSetComputeUnitPrice:
            return parse_compute_budget_unit_price_instruction(&parser, instruction, header, &info->set_compute_unit_price);
        // Unsupported instruction
        case ComputeBudgetRequestUnitsDeprecated:
            break;
    }

    return 1;
}

static int print_compute_budget_heap_frame_info(const ComputeBudgetRequestHeapFrameInfo* info,
                                                const PrintConfig* print_config) {
    UNUSED(print_config);

    SummaryItem* item;

    item = transaction_summary_primary_item();
    summary_item_set_amount(item, "Heap size", info->bytes);

    return 0;
}

static int print_compute_budget_unit_limit_info(const ComputeBudgetSetComputeUnitLimitInfo* info,
                                                const PrintConfig* print_config) {
    UNUSED(print_config);

    SummaryItem* item;

    item = transaction_summary_primary_item();
    summary_item_set_amount(item, "Compute units", info->units);

    return 0;
}

static int print_compute_budget_unit_price_info(const ComputeBudgetSetComputeUnitPriceInfo* info,
                                                const PrintConfig* print_config) {
    UNUSED(print_config);

    SummaryItem* item;

    item = transaction_summary_primary_item();
    summary_item_set_amount(item, "Micro-lamports", info->micro_lamports);

    return 0;
}

int print_compute_budget_info(const ComputeBudgetInfo* info, const PrintConfig* print_config) {
    switch (info->kind) {
        case ComputeBudgetRequestHeapFrame:
            return print_compute_budget_heap_frame_info(&info->request_heap_frame, print_config);
        case ComputeBudgetSetComputeUnitLimit:
            return print_compute_budget_unit_limit_info(&info->set_compute_unit_limit, print_config);
        case ComputeBudgetSetComputeUnitPrice:
            return print_compute_budget_unit_price_info(&info->set_compute_unit_price, print_config);
        // Unsupported instruction
        case ComputeBudgetRequestUnitsDeprecated:
            break;
    }

    return 1;
}
