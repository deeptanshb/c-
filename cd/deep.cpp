#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

mutex resourceLock;

struct VM {
    int id;
    double cpuRequest;
    double ramRequest;
    double gpuRequest;
};

void allocateResources(vector<VM> &vms, double &cpuAvailable, double &ramAvailable, double &gpuAvailable) {
    bool equilibriumReached = false;

    while (!equilibriumReached) {
        lock_guard<mutex> lock(resourceLock);

        double totalCPURequest = 0, totalRAMRequest = 0, totalGPURequest = 0;

        for (const auto &vm : vms) {
            totalCPURequest += vm.cpuRequest;
            totalRAMRequest += vm.ramRequest;
            totalGPURequest += vm.gpuRequest;
        }

        
        double cpuScale = (totalCPURequest > cpuAvailable) ? (cpuAvailable / totalCPURequest) : 1.0;
        double ramScale = (totalRAMRequest > ramAvailable) ? (ramAvailable / totalRAMRequest) : 1.0;
        double gpuScale = (totalGPURequest > gpuAvailable) ? (gpuAvailable / totalGPURequest) : 1.0;

        
        static bool printedScaling = false;
        if (!printedScaling) {
            cout << "\nScaling Factors:\n";
            cout << "CPU Scaling Factor: " << cpuScale << endl;
            cout << "RAM Scaling Factor: " << ramScale << endl;
            cout << "GPU Scaling Factor: " << gpuScale << endl;
            printedScaling = true;
        }

        equilibriumReached = true;
        for (auto &vm : vms) {
            double adjustedCPU = vm.cpuRequest * cpuScale;
            double adjustedRAM = vm.ramRequest * ramScale;
            double adjustedGPU = vm.gpuRequest * gpuScale;

            if (fabs(vm.cpuRequest - adjustedCPU) > 0.01 ||
                fabs(vm.ramRequest - adjustedRAM) > 0.01 ||
                fabs(vm.gpuRequest - adjustedGPU) > 0.01) {
                equilibriumReached = false;
            }

            vm.cpuRequest = round(adjustedCPU);
            vm.ramRequest = round(adjustedRAM);
            vm.gpuRequest = round(adjustedGPU);
        }

        
        cpuAvailable = 0;
        ramAvailable = 0;
        gpuAvailable = 0;

        for (const auto &vm : vms) {
            cpuAvailable += vm.cpuRequest;
            ramAvailable += vm.ramRequest;
            gpuAvailable += vm.gpuRequest;
        }
    }
}

void vmThread(VM &vm, vector<VM> &vms, double &cpuAvailable, double &ramAvailable, double &gpuAvailable) {
    allocateResources(vms, cpuAvailable, ramAvailable, gpuAvailable);
}

int main() {
    int numVMs;
    double totalCPU, totalRAM, totalGPUs;

    cout << "Enter number of VMs: ";
    cin >> numVMs;
    cout << "Enter total CPU units: ";
    cin >> totalCPU;
    cout << "Enter total RAM units: ";
    cin >> totalRAM;
    cout << "Enter total GPU units: ";
    cin >> totalGPUs;

    vector<thread> vmThreads;
    vector<VM> vms;

    srand(time(0));

    double totalCPUReq = 0, totalRAMReq = 0, totalGPUReq = 0;

    cout << "\nInitial Resource Requests:\n";
    for (int i = 0; i < numVMs; i++) {
        double cpuReq = (rand() % static_cast<int>(totalCPU / 2)) + 1;
        double ramReq = (rand() % static_cast<int>(totalRAM / 2)) + 1;
        double gpuReq = (rand() % static_cast<int>(totalGPUs / 2)) + 1;

        totalCPUReq += cpuReq;
        totalRAMReq += ramReq;
        totalGPUReq += gpuReq;

        cout << "VM " << i + 1 << " -> CPU: " << cpuReq
             << ", RAM: " << ramReq
             << ", GPU: " << gpuReq << endl;

        vms.push_back({i + 1, cpuReq, ramReq, gpuReq});
    }

    cout << "\nTotal Initial Resource Requests:\n";
    cout << "Total CPU Requested: " << totalCPUReq << endl;
    cout << "Total RAM Requested: " << totalRAMReq << endl;
    cout << "Total GPU Requested: " << totalGPUReq << endl;

    for (int i = 0; i < numVMs; i++) {
        vmThreads.push_back(thread(vmThread, ref(vms[i]), ref(vms), ref(totalCPU), ref(totalRAM), ref(totalGPUs)));
    }

    for (auto &t : vmThreads) {
        t.join();
    }

    cout << "\nFinal VM Resource Allocations (Nash Equilibrium Achieved):\n";
    for (const auto &vm : vms) {
        cout << "VM " << vm.id << " -> CPU: " << vm.cpuRequest
             << ", RAM: " << vm.ramRequest
             << ", GPU: " << vm.gpuRequest << endl;
    }

    return 0;
}

