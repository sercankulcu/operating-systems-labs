# Operating Systems Labs

Welcome to the **Operating Systems Labs** repository! This project contains a collection of lab exercises designed to explore and implement core operating system concepts. It serves as an educational resource for students, learners, and enthusiasts studying operating systems through practical programming assignments.

## Overview

This repository includes lab exercises that demonstrate key operating system concepts, such as process management, thread synchronization, memory management, and file systems. Each lab is implemented in a programming language (e.g., C or Java) and is organized in separate files or directories for clarity. The labs are designed to be hands-on, reinforcing theoretical concepts through practical implementation.

## Lab Exercises

The following lab exercises are included in this repository (organized by topic or lab number):

- **Lab 1: Process Creation and Management**
  - Explores process creation using system calls (e.g., `fork()` in C or process simulation in Java).
  - Demonstrates parent-child process relationships and process termination.

- **Lab 2: Process Scheduling**
  - Implements scheduling algorithms like Round-Robin, First-Come-First-Served (FCFS), or Shortest Job First (SJF).
  - Simulates CPU scheduling and calculates metrics like turnaround time.

- **Lab 3: Thread Synchronization**
  - Demonstrates thread creation and synchronization using semaphores, monitors, or locks.
  - Includes examples like the Producer-Consumer problem.

- **Lab 4: Memory Management**
  - Implements page replacement algorithms (e.g., FIFO, LRU) or memory allocation strategies.
  - Simulates virtual memory concepts.

- **Lab 5: Deadlock Handling**
  - Explores deadlock detection or prevention using algorithms like the Banker’s Algorithm.
  - Demonstrates resource allocation scenarios.

- **Lab 6: File Systems (if applicable)**
  - Simulates basic file system operations or disk scheduling algorithms.

Each lab is located in the `src/` directory or organized in subdirectories (e.g., `lab1/`, `lab2/`). Refer to individual lab files for specific details and instructions.

## Getting Started

### Prerequisites
- **Programming Environment**:
  - For C labs: A C compiler (e.g., `gcc`) and a POSIX-compliant system (e.g., Linux or macOS).
  - For Java labs: Java Development Kit (JDK) version 8 or higher.
- A text editor or IDE (e.g., VS Code, IntelliJ IDEA, Eclipse).
- (Optional) A Linux environment for labs involving system calls (e.g., `fork()`, `pthread`).

### Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/sercankulcu/operating-systems-labs.git
   ```
2. Navigate to the project directory:
   ```bash
   cd operating-systems-labs
   ```

3. **Compile and Run**:
   - For C labs:
     ```bash
     gcc src/lab1/process_creation.c -o process_creation
     ./process_creation
     ```
   - For Java labs:
     ```bash
     javac src/lab2/RoundRobin.java
     java RoundRobin
     ```
   - Note: Some labs may require input files or specific arguments. Check the source code or lab-specific comments for details.

### Usage
Each lab is a standalone program or set of functions demonstrating an OS concept. To run a lab:

1. Navigate to the relevant lab directory or file (e.g., `src/lab1/` or `src/ProcessScheduling.java`).
2. Compile and execute the code as described above.
3. Follow any lab-specific instructions (e.g., providing input data or command-line arguments).
4. Example (for a scheduling lab in Java):
   ```java
   RoundRobin scheduler = new RoundRobin();
   scheduler.addProcess(new Process(1, 10)); // Process ID, burst time
   scheduler.run();
   ```

Refer to the source code in `src/` or lab directories for detailed implementations and comments.

## Contributing
Contributions are welcome! If you'd like to add new labs, improve existing code, or fix bugs, please follow these steps:
1. Fork the repository.
2. Create a new branch (`git checkout -b feature/your-feature`).
3. Commit your changes (`git commit -m "Add your feature"`).
4. Push to the branch (`git push origin feature/your-feature`).
5. Open a pull request.

Please ensure your code adheres to the programming language’s conventions (e.g., C or Java) and includes clear comments or documentation.

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contact
For questions or suggestions, feel free to contact the repository owner, [Sercan Kulcu](https://github.com/sercankulcu), or open an issue on GitHub.

Happy learning!
