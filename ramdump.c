
/*first line is intentionally left blank*/
/***********************************************************************************************************/
#include <arpa/inet.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#if  defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#include <process.h>
#include <tlhelp32.h>
#include <windows.h>
#endif
#if defined(__linux__) || defined(__ANDROID__)
#include <sys/ptrace.h>
#include <sys/socket.h>
#include <sys/wait.h>
#endif
/***********************************************************************************************************/
#if  defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
DWORD access = PRCOESS_VM_READ | PROCESS_QUERY_INFORMATION | PRCOESS_VM_WRITE | PRCOESS_VM_OPERATION;
HANDLE proc = OpenProcess(access, FALSE, pid);
void dump_memory_region() {
  void* addr;
  size_t written;
  ReadProcessMemory(proc, addr, &value, sizeof(value), &written);
}

void write_memory_region(proc, addr, &value, sizeof(value), &written) {}
#endif

#if defined(__linux__) || defined(__ANDROID__)
void dump_memory_region(FILE* pMemFile, unsigned long start_address, long length, int serverSocket) {
  unsigned long address;
  int pageLength = 4096;
  unsigned char page[pageLength];
  fseeko(pMemFile, start_address, SEEK_SET);

  for (address=start_address; address < start_address + length; address += pageLength) {
    fread(&page, 1, pageLength, pMemFile);
    if (serverSocket == -1) {
      fwrite(&page, 1, pageLength, stdout);
    } else {
      send(serverSocket, &page, pageLength, 0);
    }
  }
}
#endif

int main(int argc, char **argv) {
    if (argc == 2 || argc == 4) {
      int pid = atoi(argv[1]);
      long ptraceResult = ptrace(PTRACE_ATTACH, pid, NULL, NULL);
      if (ptraceResult < 0) {
        printf("Unable to attach to the pid specified\n");
        return 1;
      }
      wait(NULL);

      char mapsFilename[1024];
      sprintf(mapsFilename, "/proc/%s/maps", argv[1]);
      FILE* pMapsFile = fopen(mapsFilename, "r");
      char memFilename[1024];
      sprintf(memFilename, "/proc/%s/mem", argv[1]);
      FILE* pMemFile = fopen(memFilename, "r");
      int serverSocket = -1;
      if (argc == 4) {   
        unsigned int port;
        int count = sscanf(argv[3], "%d", &port);
        if (count == 0) {
          printf("Invalid port specified\n");
          return 2;
        }

        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1) {
          printf("Could not create socket\n");
          return 3;
        }
        
        struct sockaddr_in serverSocketAddress;
        serverSocketAddress.sin_addr.s_addr = inet_addr(argv[2]);
        serverSocketAddress.sin_family = AF_INET;
        serverSocketAddress.sin_port = htons(port);
        if (connect(serverSocket, (struct sockaddr *) &serverSocketAddress, sizeof(serverSocketAddress)) < 0) {
          printf("Could not connect to server\n");
          return 4;
        }
      }
      char line[256];
      while (fgets(line, 256, pMapsFile) != NULL) {
        unsigned long start_address;
        unsigned long end_address;
        sscanf(line, "%08lx-%08lx\n", &start_address, &end_address);
        dump_memory_region(pMemFile, start_address, end_address - start_address, serverSocket);
      }

      fclose(pMapsFile);
      fclose(pMemFile);

      if (serverSocket != -1) {
        close(serverSocket);
      }

      ptrace(PTRACE_CONT, pid, NULL, NULL);
      ptrace(PTRACE_DETACH, pid, NULL, NULL);
  } else {
    printf("%s <pid>\n", argv[0]);
    printf("%s <pid> <ip-address> <port>\n", argv[0]);
    exit(0);
  }
}
/***********************************************************************************************************/
/*last line is intentionally left blank*/

