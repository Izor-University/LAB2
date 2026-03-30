#include <iostream>
#include <ctime>

#include "../core/LinkedList.hpp"
#include "../core/MutableArraySequence.hpp"
#include "../core/ImmutableArraySequence.hpp"
#include "../core/MutableListSequence.hpp"
#include "../core/ImmutableListSequence.hpp"
#include "../core/BitSequence.hpp"
#include "../core/Utils.hpp"

// ============================================================================
// HELPERS (TEST FUNCTIONS & OUTPUT)
// ============================================================================
template <typename T1, typename T2>
std::ostream& operator<<(std::ostream& os, const Pair<T1, T2>& p) {
    os << "(" << p.first << ", " << p.second << ")";
    return os;
}

int MapSquare(const int& x) { return x * x; }
bool WhereEven(const int& x) { return x % 2 == 0; }
int ReduceSum(const int& acc, const int& cur) { return acc + cur; }

// ============================================================================
// CLI UTILITIES
// ============================================================================
void ClearScreen() {
    // Кроссплатформенная очистка консоли через ANSI-коды
    std::cout << "\x1B[2J\x1B[H" << std::flush;
}

void WaitEnter() {
    std::cout << "\nPress Enter to continue...";
    std::cin.clear();
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

int ReadInt(const char* prompt = "") {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) return value;

        if (std::cin.eof() || std::cin.bad()) {
            std::cout << "\n[!] Critical input error. Exiting.\n";
            std::exit(1);
        }
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "  [!] Invalid input. Please enter an integer.\n";
    }
}

// ============================================================================
// REGISTRY SYSTEM (Powered by LinkedList)
// ============================================================================
struct IntSeqRecord {
    const char* typeName;
    Sequence<int>* seq;
};

struct BitSeqRecord {
    BitSequence* seq;
};

// Используем LinkedList, так как у него есть RemoveAt()
LinkedList<IntSeqRecord*> g_IntRegistry;
LinkedList<BitSeqRecord*> g_BitRegistry;

void PrintRegistries() {
    std::cout << "--- INT SEQUENCES REGISTRY ---\n";
    if (g_IntRegistry.GetLength() == 0) std::cout << "  (Empty)\n";
    for (int i = 0; i < g_IntRegistry.GetLength(); ++i) {
        IntSeqRecord* rec = g_IntRegistry.Get(i);
        std::cout << "  [" << i << "] " << rec->typeName << " : " << *(rec->seq) << "\n";
    }

    std::cout << "\n--- BIT SEQUENCES REGISTRY ---\n";
    if (g_BitRegistry.GetLength() == 0) std::cout << "  (Empty)\n";
    for (int i = 0; i < g_BitRegistry.GetLength(); ++i) {
        BitSeqRecord* rec = g_BitRegistry.Get(i);
        std::cout << "  [" << i << "] BitSequence : " << *(rec->seq) << "\n";
    }
    std::cout << "------------------------------\n";
}

int PickIntSeq() {
    if (g_IntRegistry.GetLength() == 0) return -1;
    while (true) {
        int idx = ReadInt("Select Int Sequence ID (-1 to cancel): ");
        if (idx == -1) return -1;
        if (idx >= 0 && idx < g_IntRegistry.GetLength()) return idx;
        std::cout << "  [!] ID out of bounds.\n";
    }
}

int PickBitSeq() {
    if (g_BitRegistry.GetLength() == 0) return -1;
    while (true) {
        int idx = ReadInt("Select Bit Sequence ID (-1 to cancel): ");
        if (idx == -1) return -1;
        if (idx >= 0 && idx < g_BitRegistry.GetLength()) return idx;
        std::cout << "  [!] ID out of bounds.\n";
    }
}

void RegisterIfNew(Sequence<int>* original, Sequence<int>* result, const char* typeName) {
    if (original != result) {
        g_IntRegistry.Append(new IntSeqRecord{typeName, result});
        std::cout << "  -> A NEW sequence was created and added to registry as ID ["
                  << g_IntRegistry.GetLength() - 1 << "].\n";
    } else {
        std::cout << "  -> The sequence was modified IN-PLACE.\n";
    }
}

// ============================================================================
// DELETION LOGIC
// ============================================================================
void DeleteIntSequence() {
    PrintRegistries();
    int idx = PickIntSeq();
    if (idx == -1) return;

    // Очищаем память, затем удаляем узел из связного списка
    delete g_IntRegistry.Get(idx)->seq;
    delete g_IntRegistry.Get(idx);
    g_IntRegistry.RemoveAt(idx);

    std::cout << "Sequence deleted successfully.\n";
}

void DeleteBitSequence() {
    PrintRegistries();
    int idx = PickBitSeq();
    if (idx == -1) return;

    delete g_BitRegistry.Get(idx)->seq;
    delete g_BitRegistry.Get(idx);
    g_BitRegistry.RemoveAt(idx);

    std::cout << "BitSequence deleted successfully.\n";
}

// ============================================================================
// CREATION MENUS (With Manual Input)
// ============================================================================
void CreateIntSequenceMenu() {
    std::cout << "\n--- Create Sequence<int> ---\n";
    std::cout << "1. Mutable Array Sequence\n2. Immutable Array Sequence\n";
    std::cout << "3. Mutable List Sequence\n4. Immutable List Sequence\n";
    std::cout << "0. Cancel\n";
    int type = ReadInt("Select type: ");

    Sequence<int>* seq = nullptr;
    const char* typeName = "";

    switch (type) {
        case 1: seq = new MutableArraySequence<int>(); typeName = "MutArray"; break;
        case 2: seq = new ImmutableArraySequence<int>(); typeName = "ImmArray"; break;
        case 3: seq = new MutableListSequence<int>(); typeName = "MutList"; break;
        case 4: seq = new ImmutableListSequence<int>(); typeName = "ImmList"; break;
        case 0: return;
        default: std::cout << "Invalid. Cancelled.\n"; return;
    }

    int mode = ReadInt("1. Manual Input\n2. Random Fill\nSelect mode: ");
    int count = ReadInt("How many elements? (0 for empty): ");

    for (int i = 0; i < count; ++i) {
        int val = (mode == 1) ? ReadInt("Enter value: ") : (std::rand() % 100);
        Sequence<int>* nextSeq = seq->Append(val);
        if (seq != nextSeq) { delete seq; seq = nextSeq; }
    }

    g_IntRegistry.Append(new IntSeqRecord{typeName, seq});
    std::cout << "Created! Registry updated.\n";
}

void CreateBitSequenceMenu() {
    std::cout << "\n--- Create BitSequence ---\n";
    int mode = ReadInt("1. Manual Input\n2. Random Fill\n0. Cancel\nSelect mode: ");
    if (mode == 0) return;

    int count = ReadInt("How many bits?: ");
    BitSequence* seq = new BitSequence();

    for(int i = 0; i < count; ++i) {
        int val;
        if (mode == 1) {
            while (true) {
                val = ReadInt("Enter bit (0 or 1): ");
                if (val == 0 || val == 1) break;
                std::cout << "  [!] Must be 0 or 1.\n";
            }
        } else {
            val = std::rand() % 2;
        }
        seq->Append(Bit(val != 0));
    }

    g_BitRegistry.Append(new BitSeqRecord{seq});
    std::cout << "Created! Registry updated.\n";
}

// ============================================================================
// CONTEXTUAL OPERATIONS MENUS
// ============================================================================
void IntSequenceOperations() {
    PrintRegistries();
    int idx = PickIntSeq();
    if (idx == -1) return;

    // Входим в контекстное меню (цикл)
    while (true) {
        ClearScreen();
        // ВАЖНО: Получаем указатели на каждой итерации (объект мог быть заменен в Immutable)
        IntSeqRecord* record = g_IntRegistry.Get(idx);
        Sequence<int>* s = record->seq;

        std::cout << "\n=== Operations for [" << idx << "] ===\n";
        std::cout << "Current Sequence: " << *s << "\n";
        std::cout << "---------------------------------\n";
        std::cout << " 1. Append        2. Prepend      3. InsertAt\n";
        std::cout << " 4. GetFirst      5. GetLast      6. TryGet\n";
        std::cout << " 7. Map (x^2)     8. Where (Even) 9. Reduce (Sum)\n";
        std::cout << "10. Concat       11. Slice       12. Split\n";
        std::cout << "13. Zip & Unzip\n";
        std::cout << " 0. Back to Main Menu\n";

        int op = ReadInt("> ");
        if (op == 0) break; // Выход из контекстного меню

        try {
            switch (op) {
                case 1: {
                    int val = ReadInt("Value to Append: ");
                    RegisterIfNew(s, s->Append(val), record->typeName);
                    break;
                }
                case 2: {
                    int val = ReadInt("Value to Prepend: ");
                    RegisterIfNew(s, s->Prepend(val), record->typeName);
                    break;
                }
                case 3: {
                    int val = ReadInt("Value: ");
                    int pos = ReadInt("Index: ");
                    RegisterIfNew(s, s->InsertAt(val, pos), record->typeName);
                    break;
                }
                case 4: std::cout << "First Element: " << s->GetFirst() << "\n"; break;
                case 5: std::cout << "Last Element: " << s->GetLast() << "\n"; break;
                case 6: {
                    Option<int> opt = s->TryGet(ReadInt("Index: "));
                    if (opt.IsSome()) std::cout << "Found: " << opt.GetValue() << "\n";
                    else std::cout << "Element not found (None).\n";
                    break;
                }
                case 7: RegisterIfNew(s, s->Map(MapSquare), record->typeName); break;
                case 8: RegisterIfNew(s, s->Where(WhereEven), record->typeName); break;
                case 9: std::cout << "Sum: " << s->Reduce(ReduceSum, 0) << "\n"; break;
                case 10: {
                    int otherIdx = PickIntSeq();
                    if (otherIdx == -1) break;
                    Sequence<int>* res = s->Concat(g_IntRegistry.Get(otherIdx)->seq);
                    RegisterIfNew(s, res, record->typeName);
                    break;
                }
                case 11: {
                    int start = ReadInt("Start Index: ");
                    int count = ReadInt("Count: ");
                    RegisterIfNew(s, s->Slice(start, count), record->typeName);
                    break;
                }
                case 12: {
                    int splitVal = ReadInt("Value to split by: ");
                    Sequence<Sequence<int>*>* chunks = SequenceUtils::Split(*s, splitVal);
                    std::cout << "Split into " << chunks->GetLength() << " chunks. Adding to registry...\n";
                    for (int i = 0; i < chunks->GetLength(); ++i) {
                        g_IntRegistry.Append(new IntSeqRecord{"MutArray (Split Chunk)", chunks->Get(i)});
                    }
                    delete chunks;
                    break;
                }
                case 13: {
                    int otherIdx = PickIntSeq();
                    if (otherIdx == -1) break;
                    Sequence<Pair<int, int>>* zipped = SequenceUtils::Zip(*s, *(g_IntRegistry.Get(otherIdx)->seq));
                    std::cout << "Zipped Result: " << *zipped << "\n";

                    std::cout << "\nUnzipping back...\n";
                    Pair<Sequence<int>*, Sequence<int>*> unzipped = SequenceUtils::Unzip(*zipped);
                    g_IntRegistry.Append(new IntSeqRecord{"MutArray (Unzipped 1)", unzipped.first});
                    g_IntRegistry.Append(new IntSeqRecord{"MutArray (Unzipped 2)", unzipped.second});
                    std::cout << "Unzipped sequences added to registry!\n";
                    delete zipped;
                    break;
                }
                default: std::cout << "Invalid operation.\n";
            }
        } catch (const Exception& e) {
            std::cout << "  [EXCEPTION]: " << e.GetMessage() << "\n";
        }
        WaitEnter(); // Ждем ввода перед очисткой экрана и новым витком меню
    }
}

void BitSequenceOperations() {
    PrintRegistries();
    int idx = PickBitSeq();
    if (idx == -1) return;

    while (true) {
        ClearScreen();
        BitSequence* b = g_BitRegistry.Get(idx)->seq;

        std::cout << "\n=== BitSequence Ops for [" << idx << "] ===\n";
        std::cout << "Current Sequence: " << *b << "\n";
        std::cout << "-------------------------------------\n";
        std::cout << "1. Append Bit    2. Prepend Bit   3. Get Size\n";
        std::cout << "4. Bit AND       5. Bit OR        6. Bit XOR\n";
        std::cout << "7. Bit NOT\n";
        std::cout << "0. Back to Main Menu\n";

        int op = ReadInt("> ");
        if (op == 0) break; // Выход

        try {
            switch (op) {
                case 1: {
                    int val;
                    while (true) {
                        val = ReadInt("Bit (0/1): ");
                        if (val == 0 || val == 1) break;
                    }
                    b->Append(Bit(val != 0));
                    std::cout << "Appended IN-PLACE.\n";
                    break;
                }
                case 2: {
                    int val;
                    while (true) {
                        val = ReadInt("Bit (0/1): ");
                        if (val == 0 || val == 1) break;
                    }
                    b->Prepend(Bit(val != 0));
                    std::cout << "Prepended IN-PLACE.\n";
                    break;
                }
                case 3: std::cout << "Size: " << b->GetLength() << "\n"; break;
                case 4: {
                    int otherIdx = PickBitSeq();
                    if (otherIdx == -1) break;
                    BitSequence* res = b->And(g_BitRegistry.Get(otherIdx)->seq);
                    g_BitRegistry.Append(new BitSeqRecord{res});
                    std::cout << "Result added as NEW BitSequence to registry.\n";
                    break;
                }
                case 5: {
                    int otherIdx = PickBitSeq();
                    if (otherIdx == -1) break;
                    BitSequence* res = b->Or(g_BitRegistry.Get(otherIdx)->seq);
                    g_BitRegistry.Append(new BitSeqRecord{res});
                    std::cout << "Result added as NEW BitSequence to registry.\n";
                    break;
                }
                case 6: {
                    int otherIdx = PickBitSeq();
                    if (otherIdx == -1) break;
                    BitSequence* res = b->Xor(g_BitRegistry.Get(otherIdx)->seq);
                    g_BitRegistry.Append(new BitSeqRecord{res});
                    std::cout << "Result added as NEW BitSequence to registry.\n";
                    break;
                }
                case 7: {
                    g_BitRegistry.Append(new BitSeqRecord{b->Not()});
                    std::cout << "Result added as NEW BitSequence to registry.\n";
                    break;
                }
                default: std::cout << "Invalid operation.\n";
            }
        } catch (const Exception& e) {
            std::cout << "  [EXCEPTION]: " << e.GetMessage() << "\n";
        }
        WaitEnter();
    }
}

// ============================================================================
// MAIN LOOP & CLEANUP
// ============================================================================
void CleanupMemory() {
    for (int i = 0; i < g_IntRegistry.GetLength(); ++i) {
        delete g_IntRegistry.Get(i)->seq;
        delete g_IntRegistry.Get(i);
    }
    for (int i = 0; i < g_BitRegistry.GetLength(); ++i) {
        delete g_BitRegistry.Get(i)->seq;
        delete g_BitRegistry.Get(i);
    }
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    while (true) {
        ClearScreen();
        PrintRegistries();

        std::cout << "\n=== MAIN MENU ===\n";
        std::cout << "1. Create Sequence<int>\n";
        std::cout << "2. Create BitSequence\n";
        std::cout << "3. Int Sequence Operations (Context Menu)\n";
        std::cout << "4. Bit Sequence Operations (Context Menu)\n";
        std::cout << "5. Delete Int Sequence\n";
        std::cout << "6. Delete Bit Sequence\n";
        std::cout << "0. Exit Program\n";

        int choice = ReadInt("> ");

        switch (choice) {
            case 1: CreateIntSequenceMenu(); WaitEnter(); break;
            case 2: CreateBitSequenceMenu(); WaitEnter(); break;
            case 3: IntSequenceOperations(); break; // WaitEnter внутри цикла
            case 4: BitSequenceOperations(); break; // WaitEnter внутри цикла
            case 5: DeleteIntSequence(); WaitEnter(); break;
            case 6: DeleteBitSequence(); WaitEnter(); break;
            case 0:
                CleanupMemory();
                std::cout << "Memory cleared. Goodbye!\n";
                return 0;
            default:
                std::cout << "Unknown command.\n";
                WaitEnter();
                break;
        }
    }
}