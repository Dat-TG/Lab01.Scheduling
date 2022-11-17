#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <queue>
using namespace std;
const int spacing = 30;
struct Process
{
    string Name;//Tên tiến trình, dùng để phân biệt các tiến trình
    uint32_t ArrivalTime;//Thời điểm vào
    uint32_t CPU_Brust;//Thời gian chạy
    uint32_t Priority = 0;//Độ ưu tiên
    uint32_t StartTime = 0;//Thời điểm bắt đầu chạy
    uint32_t EndTime = 0;//Thời điểm chạy xong
    uint32_t PauseTime = 0;//Thời điểm dừng trước đó
    uint32_t TT = 0;//Thời gian tồn tại trong hệ thống
    uint32_t WT = 0;//Thời gian chờ
    uint32_t ID;//ID của tiến trình, để phân biệt các tiến trình
};
void input(string filename, vector<Process>& ProcessList, uint32_t& numberOfProcess, uint32_t& quantum)
{
    ifstream fi(filename);
    string s;
    getline(fi, s);
    stringstream ss(s);
    ss>>numberOfProcess>>quantum;
    for (uint32_t i = 0; i < numberOfProcess; i++)
    {
        getline(fi, s);
        stringstream ss(s);
        Process P;
        ss >> P.Name;
        ss >> P.ArrivalTime;
        ss >> P.CPU_Brust;
        ss >> P.Priority;
        ProcessList.push_back(P);
    }
}
void FCFS(string output, vector<Process>& ProcessList, const uint32_t& numberOfProcess) {
    ofstream fo(output);
    fo << " Scheduling chart: ";
    ProcessList[0].StartTime = ProcessList[0].ArrivalTime;
    ProcessList[0].EndTime = ProcessList[0].CPU_Brust + ProcessList[0].StartTime;
    fo << ProcessList[0].StartTime << "~" << ProcessList[0].Name << "~" << ProcessList[0].EndTime;
    for (uint32_t i = 1; i < numberOfProcess; i++) {
        ProcessList[i].StartTime = max(ProcessList[i].ArrivalTime, ProcessList[i - 1].EndTime);
        ProcessList[i].EndTime = ProcessList[i].StartTime + ProcessList[i].CPU_Brust;
        if (ProcessList[i].StartTime > ProcessList[i - 1].EndTime) {
            fo << "~" << ProcessList[i].StartTime;
        }
        fo << "~" << ProcessList[i].Name << "~" << ProcessList[i].EndTime;
    }
    fo << endl;
    double avgTT = 0, avgWT = 0;
    for (uint32_t i = 0; i < numberOfProcess; i++) {
        ProcessList[i].TT = ProcessList[i].EndTime - ProcessList[i].ArrivalTime;
        ProcessList[i].WT = ProcessList[i].StartTime - ProcessList[i].ArrivalTime;
        avgTT += ProcessList[i].TT;
        avgWT += ProcessList[i].WT;
        fo << " "<<setw(spacing) << left << ProcessList[i].Name + ":" << setw(spacing) << left << "TT = " + to_string(ProcessList[i].TT) << setw(spacing) << left << "WT = " + to_string(ProcessList[i].WT) << endl;
    }
    avgTT /= (double)(numberOfProcess);
    avgWT /= (double)(numberOfProcess);
    fo << " " << setw(spacing) << left << "Average:" << setw(spacing) << left << "TT = " + to_string(avgTT) << setw(spacing) << left << "WT = " + to_string(avgWT);
    fo.close();
}
void RR(string output, vector<Process>& ProcessList, const uint32_t& numberOfProcess, uint32_t quantum) {
    ofstream fo(output);
    fo << " Scheduling chart: ";
    double avgTT = 0, avgWT = 0;
    ProcessList[0].StartTime = ProcessList[0].ArrivalTime;
    queue<Process>q;
    for (uint32_t i = 0; i < numberOfProcess; i++) {
        ProcessList[i].PauseTime = ProcessList[i].ArrivalTime;
    }
    q.push(ProcessList[0]);
    uint32_t pos = 1;
    uint32_t current = ProcessList[0].ArrivalTime;
    fo << current << "~" << ProcessList[0].Name<<"~";
    vector<uint32_t>WT(numberOfProcess, 0);//Thời gian chờ của các Process
    vector<uint32_t>TT(numberOfProcess, 0);//Thời gian tồn tại trong hệ thống của các Process
    while (!q.empty() || pos<numberOfProcess) {
        if (pos < numberOfProcess) {
            for (uint32_t i = pos; i < numberOfProcess; i++) {
                if (ProcessList[i].ArrivalTime <= current+quantum) {
                    q.push(ProcessList[i]);
                    pos++;
                }
            }
        }
        Process p = q.front();
        avgWT += current - p.PauseTime;
        WT[p.ID] += current - p.PauseTime;
        if (p.StartTime == 0) {
            p.StartTime = current;
        }
        if (q.size() == 1 && pos>=numberOfProcess) {
                fo << current + p.CPU_Brust;
                current += p.CPU_Brust;
                p.CPU_Brust = 0;
                p.EndTime = current;
                q.pop();
                avgTT += current - p.ArrivalTime;
                TT[p.ID] = current - p.ArrivalTime;
                break;
        }
        else {
            if (p.CPU_Brust > quantum) {
                p.CPU_Brust -= quantum;
                current += quantum;
                p.PauseTime = current;
                q.pop();
                q.push(p);
            }
            else {
                current += p.CPU_Brust;
                p.CPU_Brust = 0;
                p.PauseTime = current;
                avgTT += current - p.ArrivalTime;
                TT[p.ID] = current - p.ArrivalTime;
                q.pop();
            }
        }
        if (q.empty() && pos >= numberOfProcess) {
            break;
        }
        fo <<current;
        if (q.empty() && pos < numberOfProcess) {
            current = ProcessList[pos].ArrivalTime;
            q.push(ProcessList[pos]);
            fo <<"~"<<ProcessList[pos].ArrivalTime;
            pos++;
        }
        if (!q.empty()) {
            fo << "~"<<q.front().Name << "~";
        }
    }
    fo << endl;
    for (uint32_t i = 0; i < numberOfProcess; i++) {
        fo << " " << setw(spacing) << left << ProcessList[i].Name + ":" << setw(spacing) << left << "TT = " + to_string(TT[i]) << setw(spacing) << left << "WT = " + to_string(WT[i]) << endl;
    }
    avgTT /= (double)(numberOfProcess);
    avgWT /= (double)(numberOfProcess);
    fo << " " << setw(spacing) << left << "Average:" << setw(spacing) << left << "TT = " + to_string(avgTT) << setw(spacing) << left << "WT = " + to_string(avgWT);
    fo.close();
}
bool cmp(const Process& a, const Process& b) {//compare dùng để sắp xếp danh sách tiến trình tăng dần theo Thời điểm vào
    return (a.ArrivalTime < b.ArrivalTime);
}
int main()
{
    vector<Process>ProcessList;
    uint32_t numberOfProcess, quantum;
    input("Input.txt",ProcessList, numberOfProcess, quantum);
    sort(ProcessList.begin(), ProcessList.end(), cmp);
    for (uint32_t i = 0; i < numberOfProcess; i++) {
        ProcessList[i].ID = i;//Đặt ID
    }
    FCFS("FCFS.txt",ProcessList, numberOfProcess);
    RR("RR.txt", ProcessList, numberOfProcess, quantum);
    return 0;
}