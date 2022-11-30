#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <queue>
#include <algorithm>
using namespace std;
const uint32_t spacing = 30;
bool  executingSRTN = false;
bool executingPriority = false;
struct Process
{
    string Name;//Tên tiến trình, dùng để phân biệt các tiến trình
    uint32_t ArrivalTime;//Thời điểm vào
    uint32_t CPU_Brust;//Thời gian chạy
    uint32_t Priority;//Độ ưu tiên
    uint32_t RemainingTime; //Thời gian còn lại (Bổ sung)
    uint32_t StartTime = 0;//Thời điểm bắt đầu chạy
    uint32_t EndTime = 0;//Thời điểm chạy xong
    uint32_t PauseTime = 0;//Thời điểm dừng trước đó
    uint32_t TT = 0;//Thời gian tồn tại trong hệ thống
    uint32_t WT = 0;//Thời gian chờ
    uint32_t ID;//ID của tiến trình, để phân biệt các tiến trình

    bool operator< (Process second) const //(Bổ sung)
    {
        if (executingSRTN == true)
            return RemainingTime < second.RemainingTime;
        else if (executingPriority == true)
            return Priority < second.Priority;
    }
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
void SRTN(string output, vector<Process> ProcessList)
{
    ofstream fo(output);
    fo << " Scheduling chart: ";

    double avgTT = 0, avgWT = 0;

    if (ProcessList.size() == 0)
    {
        return;
    }

    executingSRTN = true;

    vector<Process> SRTN2;
    vector<Process> remainingProcessList;
    vector<Process> ready;
    vector<uint32_t> processChangeTime; //for gantt chart output
    vector<string> processName; //for gantt chart output

    Process running;
    bool changeProcess = false;
    uint32_t totalBurst = 0;
    uint32_t executionTime = 0;

    for (uint32_t i = 0; i < ProcessList.size(); i++)
    {
        SRTN2.push_back(ProcessList[i]);
        remainingProcessList.push_back(ProcessList[i]);
    }

    for (uint32_t i = 0; i < SRTN2.size(); i++)
    {
        totalBurst = totalBurst + SRTN2[i].CPU_Brust;
    }
    uint32_t time = 0;

    while (remainingProcessList.size() != 0)
    {
        for (uint32_t i = 0; i < SRTN2.size(); i++)
        {
            if (time == SRTN2[i].ArrivalTime)
            {
                if (SRTN2[i].CPU_Brust < running.RemainingTime)
                {
                    ready.push_back(running);
                    running = SRTN2[i];
                    changeProcess = true;
                }
                else
                {
                    ready.push_back(SRTN2[i]);
                }
                sort(ready.begin(), ready.end());
            }
        }

        if (changeProcess == true)
        {
            if (running.Name != "")
                processName.push_back(running.Name);
            else
                processName.push_back("W ");

            processChangeTime.push_back(time);

            changeProcess = false;
        }
        if (running.RemainingTime >= 2)
        {
            running.RemainingTime -= 1;
        }
        else if (running.RemainingTime == 1)
        {
            changeProcess = true;

            for (uint32_t i = 0; i < remainingProcessList.size(); i++)
            {
                if (running.Name == remainingProcessList[i].Name)
                {
                    if (remainingProcessList.size() > 1)
                        remainingProcessList.erase(remainingProcessList.begin() + i);
                    else
                    {
                        remainingProcessList.pop_back();
                        processChangeTime.push_back(time + 1);
                        changeProcess = false;
                        executingSRTN = false;
                        executionTime = time + 1;
                    }
                }
            }

            for (uint32_t i = 0; i < SRTN2.size(); i++)
            {
                if (running.Name == SRTN2[i].Name)
                {
                    SRTN2[i].EndTime = time + 1;
                }
            }
            running = ready[0];


            if (ready.size() != 0)
                ready.erase(ready.begin());
        }

        if (ready.size() != 0)
        {
            if (running.RemainingTime > ready[0].RemainingTime)
            {
                running = ready[0];
                ready.erase(ready.begin());
                changeProcess = true;
            }
        }
        time++;
    }

    if (processName.size() != 0)
    {
        fo << processChangeTime[0] << " ~ ";
        for (uint32_t i = 1; i < processChangeTime.size() - 1; i++)
        {
            fo << processName[i - 1] << " ~ ";
            fo << processChangeTime[i] << " ~ ";
        }
        fo << processName[processChangeTime.size() - 2] << " ~ ";
        fo << processChangeTime[processChangeTime.size() - 1] << endl;

        for (uint32_t i = 0; i < SRTN2.size(); i++)
        {
            SRTN2[i].TT = SRTN2[i].EndTime - SRTN2[i].ArrivalTime;
            SRTN2[i].WT = SRTN2[i].TT - SRTN2[i].CPU_Brust;
            fo << " " << setw(spacing) << left << SRTN2[i].Name + ":" << setw(spacing) << left << "TT = " + to_string(SRTN2[i].TT) << setw(spacing) << left << "WT = " + to_string(SRTN2[i].WT) << endl;
            avgTT += SRTN2[i].TT;
            avgWT += SRTN2[i].WT;
        }

        avgTT = avgTT / SRTN2.size();
        avgWT = avgWT / SRTN2.size();
        fo << " " << setw(spacing) << left << "Average:" << setw(spacing) << left << "TT = " + to_string(avgTT) << setw(spacing) << left << "WT = " + to_string(avgWT);

    }
}

void Priority(string output, vector<Process> ProcessList)
{
    ofstream fo(output);
    fo << " Scheduling chart: "; //ghi file

    double avgTT = 0, avgWT = 0;

    if (ProcessList.size() == 0)
    {
        return;
    }

    executingPriority = true;

    vector<Process> Priority2;
    vector<Process> remainingProcessList;
    vector<Process> ready;
    vector<uint32_t> processChangeTime;
    vector<string> processName;

    Process running;
    bool changeProcess = false;
    uint32_t totalBurst = 0;
    uint32_t executionTime = 0;

    for (uint32_t i = 0; i < ProcessList.size(); i++)
    {
        Priority2.push_back(ProcessList[i]);
        remainingProcessList.push_back(ProcessList[i]);
    }

    for (uint32_t i = 0; i < Priority2.size(); i++)
    {
        totalBurst = totalBurst + Priority2[i].CPU_Brust;
    }
    uint32_t time = 0;
    while (remainingProcessList.size() != 0)
    {
        for (uint32_t i = 0; i < Priority2.size(); i++)
        {
            if (time == Priority2[i].ArrivalTime)
            {
                if (Priority2[i].Priority < running.Priority)
                {
                    ready.push_back(running);
                    running = Priority2[i];
                    changeProcess = true;
                }
                else if (Priority2[i].Priority == running.Priority)
                {
                    if (Priority2[i].CPU_Brust < running.RemainingTime)
                    {
                        ready.push_back(running);
                        running = Priority2[i];
                        changeProcess = true;
                    }
                    else if (Priority2[i].CPU_Brust == running.RemainingTime)
                    {
                        ready.push_back(Priority2[i]);
                    }
                }
                else
                {
                    ready.push_back(Priority2[i]);
                }
                if (ready.size() >= 2)
                {
                    sort(ready.begin(), ready.end());
                }
            }
        }

        if (changeProcess == true)
        {
            if (running.Name != "")
                processName.push_back(running.Name);
            else
                processName.push_back("W ");

            processChangeTime.push_back(time);

            changeProcess = false;
        }
        if (running.RemainingTime >= 2)
        {
            running.RemainingTime -= 1;
        }
        else if (running.RemainingTime == 1)
        {
            changeProcess = true;

            for (uint32_t i = 0; i < remainingProcessList.size(); i++)
            {
                if (running.Name == remainingProcessList[i].Name)
                {
                    if (remainingProcessList.size() > 1)
                        remainingProcessList.erase(remainingProcessList.begin() + i);
                    else
                    {
                        remainingProcessList.pop_back();
                        processChangeTime.push_back(time + 1);
                        changeProcess = false;
                        executionTime = time + 1;
                        executingPriority = false;
                    }
                }
            }

            for (uint32_t i = 0; i < Priority2.size(); i++)
            {
                if (running.Name == Priority2[i].Name)
                {
                    Priority2[i].EndTime = time + 1;
                }
            }
            running = ready[0];

            if (ready.size() != 0)
                ready.erase(ready.begin());
        }

        if (ready.size() != 0)
        {
            if (running.Priority > ready[0].Priority)
            {
                if (running.RemainingTime > ready[0].RemainingTime)
                {
                    running = ready[0];
                    ready.erase(ready.begin());
                    changeProcess = true;
                }
            }
        }
        time++;
    }

    if (processName.size() != 0)
    {
        fo << processChangeTime[0] << " ~ ";
        for (uint32_t i = 1; i < processChangeTime.size() - 1; i++)
        {
            fo << processName[i - 1] << " ~ "; //ghi file
            fo << processChangeTime[i] << " ~ "; //ghi file
        }
        fo << processName[processChangeTime.size() - 2] << " ~ "; //ghi file
        fo << processChangeTime[processChangeTime.size() - 1] << endl; //ghi file

        for (uint32_t i = 0; i < Priority2.size(); i++)
        {
            Priority2[i].TT = Priority2[i].EndTime - Priority2[i].ArrivalTime;
            Priority2[i].WT = Priority2[i].TT - Priority2[i].CPU_Brust;
            fo << " " << setw(spacing) << left << Priority2[i].Name + ":" << setw(spacing) << left << "TT = " + to_string(Priority2[i].TT) << setw(spacing) << left << "WT = " + to_string(Priority2[i].WT) << endl; //ghi file
            avgTT += Priority2[i].TT;
            avgWT += Priority2[i].WT;
        }

        avgTT = avgTT / Priority2.size();
        avgWT = avgWT / Priority2.size();
        fo << " " << setw(spacing) << left << "Average:" << setw(spacing) << left << "TT = " + to_string(avgTT) << setw(spacing) << left << "WT = " + to_string(avgWT); //ghi file

    }
}
uint32_t main()
{
    vector<Process>ProcessList;
    uint32_t numberOfProcess, quantum;
    input("Input.txt", ProcessList, numberOfProcess, quantum);
    sort(ProcessList.begin(), ProcessList.end(), cmp);
    for (uint32_t i = 0; i < numberOfProcess; i++) {
        ProcessList[i].ID = i;//Đặt ID
        ProcessList[i].RemainingTime = ProcessList[i].CPU_Brust; // (Bổ sung) 
    }
    SRTN("SRTN.txt", ProcessList);
    Priority("Priority.txt", ProcessList);
    FCFS("FCFS.txt",ProcessList, numberOfProcess);
    RR("RR.txt", ProcessList, numberOfProcess, quantum);
    return 0;
}