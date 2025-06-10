# 🛒 Supermarket Management System (C)

A modular command-line program written in C for managing supermarket operations including customers, products, orders, and more. Built as a hands-on academic project, it demonstrates practical use of C programming, dynamic memory, data structures, file I/O, and bitwise operations for binary file compression.
---

## 📂 Project Structure

This project uses a multi-file, modular architecture with `.c` and `.h` files organized by responsibility.

```
├── main.c
├── Customer.c / Customer.h
├── ClubMember.c / ClubMember.h
├── Product.c / Product.h
├── ShoppingCart.c / ShoppingCart.h
├── ShoppingItem.c / ShoppingItem.h
├── FileHelper.c / FileHelper.h
├── Supermarket.c / Supermarket.h
├── SuperFile.c / SuperFile.h
├── General.c / General.h
├── GeneralList.c / GeneralList.h
├── Date.c / Date.h
├── Variadics.c / Variadics.h
├── myMacros.h
├── vtable.h
├── SupermarketProject.sln  ← Visual Studio solution file
└── README.md
```

---

## 🔧 Features

- Add/manage customers and club members  
- Manage products and inventory  
- Add/remove items to shopping carts  
- Save/load supermarket data in binary format  
- Optional compressed save mode  
- Modular design using header/source files  
- Robust memory management using pointers and dynamic allocation

---

## 💻 Technologies Used

- C (Standard C99)
- Structs, pointers, arrays, and linked lists
- File I/O (binary & compressed modes)
- Modular project structure
- Command-line arguments

---

## 🏁 How to Run This Project

### 🖥 Requirements

- [Visual Studio 2022](https://visualstudio.microsoft.com/) (Community edition is free)
- “Desktop development with C++” workload installed

---

### 🚀 Steps to Build & Run

1. **Open the solution file**  
   ```
   SupermarketProject.sln
   ```

2. In Visual Studio, go to:  
   **Debug → SupermarketProject Debug Properties**

3. Navigate to:  
   **Configuration Properties → Debugging**

4. In **Command Arguments**, enter one of the following:

   - To run in **regular mode**:
     ```
     0 SuperMarket.bin
     ```

   - To run in **compressed mode**:
     ```
     1 SuperMarket_Compress.bin
     ```

5. Click **Apply**, then **OK**

6. Press **Ctrl + F5** to build and run without debugging

---

## 📁 Binary Files

- `SuperMarket.bin` – Standard binary save/load  
- `SuperMarket_Compress.bin` – Compressed binary version using bit-level operations

---

## ✏️ Author

**Gal Grinfeld**  
Computer Science Student @ Afeka College of Engineering  
[GitHub Profile](https://github.com/galgrinfeld)  
[LinkedIn](https://www.linkedin.com/in/gal-grinfeld/)

---

## 📜 License

This project is for academic and personal learning purposes.  
Feel free to use it as reference or for self-study.
