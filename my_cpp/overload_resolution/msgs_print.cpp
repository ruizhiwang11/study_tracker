#include <print>
#include <iostream>
#include <sstream>

// ══════════════════════════════════════════════════════════════════════════════
// PROBLEM: print defined outside the struct's namespace
//
// ADL for print(os, msg) searches:
//   - std namespace    (from os argument)  → not there
//   - msgs namespace   (from msg argument) → only finds what is actually in msgs
//
// If print is defined globally, ADL on the msg argument searches msgs and finds
// nothing. Unqualified lookup may or may not reach the global definition
// depending on where the call site is — inconsistent, fragile.
// ══════════════════════════════════════════════════════════════════════════════

namespace msgs_bad {
    struct OrderMsg  { int id;   };
    struct TradeMsg  { int qty;  };
    struct RejectMsg { int code; };
    // print NOT defined here — lives outside in global scope (bad)
}

// defined globally — ADL on msgs_bad::OrderMsg searches msgs_bad, not global
void print(std::ostream& os, msgs_bad::OrderMsg m)  { os << "Order id="  << m.id;   }
void print(std::ostream& os, msgs_bad::TradeMsg m)  { os << "Trade qty=" << m.qty;  }
// RejectMsg forgotten entirely — easy to miss when structs are spread across files


// ══════════════════════════════════════════════════════════════════════════════
// FIX: hidden friend — print lives inside the struct, in the right namespace
//
// ADL for print(os, msg):
//   msg is msgs::OrderMsg → searches msgs namespace → finds the hidden friend ✓
//
// Each struct carries its own print. You cannot forget one — the definition is
// next to the struct definition. No namespace mismatch possible.
// ══════════════════════════════════════════════════════════════════════════════

namespace msgs {

    struct OrderMsg {
        int id;
        int price;
        int qty;

        // hidden friend: lives in msgs namespace, only reachable via ADL
        friend void print(std::ostream& os, OrderMsg m) {
            os << "OrderMsg { id=" << m.id
               << " price=" << m.price
               << " qty=" << m.qty << " }";
        }
    };

    struct TradeMsg {
        int order_id;
        int exec_qty;
        int exec_price;

        friend void print(std::ostream& os, TradeMsg m) {
            os << "TradeMsg { order_id=" << m.order_id
               << " exec_qty=" << m.exec_qty
               << " exec_price=" << m.exec_price << " }";
        }
    };

    struct RejectMsg {
        int order_id;
        int code;

        friend void print(std::ostream& os, RejectMsg m) {
            os << "RejectMsg { order_id=" << m.order_id
               << " code=" << m.code << " }";
        }
    };

    // ── generic dispatcher ─────────────────────────────────────────────────
    // Works for any msg type that has a hidden friend print(ostream&, T).
    // ADL fires on msg — finds the right print for each type at compile time.
    // No virtual dispatch, no type erasure, no if/else chain.

    template <typename Msg>
    void log(std::ostream& os, Msg const& msg) {
        os << "[LOG] ";
        print(os, msg);   // ADL: Msg is in msgs namespace → finds hidden friend
        os << '\n';
    }

} // namespace msgs


// ══════════════════════════════════════════════════════════════════════════════
// WHY THIS MATTERS IN HFT
//
// In exchange connectivity / market data, you typically have 20-50 message
// types (NewOrder, Cancel, Replace, Trade, Quote, Status, Reject, ...).
// A generic log/debug/dump function that works on all of them via ADL means:
//   - one template, zero per-type boilerplate at the call site
//   - adding a new message type just means adding its hidden friend print
//   - no vtable, no std::variant dispatch, no runtime cost
// ══════════════════════════════════════════════════════════════════════════════


int main()
{
    // ── bad pattern: global print ──────────────────────────────────────────
    // works here only because this call site is at global scope where the
    // global print is visible via unqualified lookup — fragile
    msgs_bad::OrderMsg bad_order{42};
    print(std::cout, bad_order);
    std::cout << '\n';

    // msgs_bad::RejectMsg bad_reject{7};
    // print(std::cout, bad_reject);  // ERROR: no matching function — forgotten

    // ── good pattern: hidden friend ────────────────────────────────────────
    msgs::OrderMsg  order  {1, 10050, 100};
    msgs::TradeMsg  trade  {1, 50, 10050};
    msgs::RejectMsg reject {1, 404};

    // direct call via ADL — each resolves to its own hidden friend
    print(std::cout, order);  std::cout << '\n';
    print(std::cout, trade);  std::cout << '\n';
    print(std::cout, reject); std::cout << '\n';

    std::cout << '\n';

    // generic log template — one function, all three types, ADL does the work
    msgs::log(std::cout, order);
    msgs::log(std::cout, trade);
    msgs::log(std::cout, reject);

    // these would both be errors — hidden friends unreachable without ADL:
    // msgs::print(std::cout, order);   // error: cannot qualify hidden friend
    // ::print(std::cout, order);       // error: global print doesn't exist for msgs::
}
