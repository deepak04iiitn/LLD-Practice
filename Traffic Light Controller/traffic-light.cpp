#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <thread>
#include <chrono>

using namespace std;


// ─── Enums ────────────────────────────────────────────────────────────────────

enum class VehicleType {
    SCOOTY,
    BIKE,
    CAR,
    AMBULANCE,
    FIRE_BRIGADE
};

enum class Signal {
    RED,
    YELLOW,
    GREEN
};

string signalToString(Signal s) {
    switch (s)
    {
        case Signal::RED:    return "RED";
        case Signal::YELLOW: return "YELLOW";
        case Signal::GREEN:  return "GREEN";
    }

    return "UNKNOWN";
}

string vehicleTypeToString(VehicleType t) {
    switch (t)
    {
        case VehicleType::SCOOTY:       return "Scooty";
        case VehicleType::BIKE:         return "Bike";
        case VehicleType::CAR:          return "Car";
        case VehicleType::AMBULANCE:    return "Ambulance";
        case VehicleType::FIRE_BRIGADE: return "Fire Brigade";
    }

    return "UNKNOWN";
}

void waitForSeconds(int sec) {
    this_thread::sleep_for(chrono::seconds(sec));
}


// ─── Observer Interface ───────────────────────────────────────────────────────

class SignalObserver {
    public:
        virtual void onSignalChange(Signal signal) = 0;
        virtual ~SignalObserver() = default;
};


// ─── Vehicle ──────────────────────────────────────────────────────────────────

class Vehicle : public SignalObserver {
private:
    string      license;
    VehicleType type;
    bool        isMoving;

public:
    Vehicle(VehicleType type, string license, bool isMoving = true)
        : type(type), license(license), isMoving(isMoving)
    {
    }

    void onSignalChange(Signal signal) override
    {
        if (signal == Signal::GREEN && !isMoving)
        {
            isMoving = true;
            cout << "  " << getTypeString() << " [" << license << "] started moving\n";
        }
        else if (signal == Signal::RED && isMoving)
        {
            isMoving = false;
            cout << "  " << getTypeString() << " [" << license << "] stopped at signal\n";
        }
        else if (signal == Signal::YELLOW)
        {
            cout << "  " << getTypeString() << " [" << license << "] slowing down\n";
        }
    }

    string getTypeString() const
    {
        return vehicleTypeToString(type);
    }

    string getLicense() const
    {
        return license;
    }

    VehicleType getType() const
    {
        return type;
    }

    bool getIsMoving() const
    {
        return isMoving;
    }

    bool isEmergency() const
    {
        return type == VehicleType::AMBULANCE || type == VehicleType::FIRE_BRIGADE;
    }
};


// ─── Road ─────────────────────────────────────────────────────────────────────

class Road {
private:
    string           roadNo;
    vector<Vehicle*> vehicles;

public:
    Road(string roadNo, vector<Vehicle*> vehicles = {})
        : roadNo(roadNo), vehicles(vehicles)
    {
    }

    void addVehicle(Vehicle* v)
    {
        vehicles.push_back(v);
    }

    void removeVehicle(const string& license)
    {
        auto it = remove_if(
            vehicles.begin(),
            vehicles.end(),
            [&](Vehicle* v)
            {
                return v->getLicense() == license;
            }
        );

        if (it != vehicles.end())
        {
            vehicles.erase(it, vehicles.end());
            cout << "  Vehicle [" << license << "] cleared " << roadNo << "\n";
        }
    }

    // Broadcast signal change to all registered vehicle observers
    void notifyAll(Signal signal)
    {
        for (auto v : vehicles)
        {
            v->onSignalChange(signal);
        }
    }

    bool hasEmergencyVehicle() const
    {
        for (auto v : vehicles)
        {
            if (v->isEmergency())
            {
                return true;
            }
        }
        return false;
    }

    string getRoadNo() const
    {
        return roadNo;
    }
};


// ─── Signal Duration ──────────────────────────────────────────────────────────

struct SignalDuration {
    int red;
    int yellow;
    int green;

    SignalDuration(int red = 10, int yellow = 3, int green = 10)
        : red(red), yellow(yellow), green(green)
    {
    }
};


// ─── Traffic Light ────────────────────────────────────────────────────────────

class TrafficLight {
private:
    Road*          road;
    Signal         currentSignal;
    SignalDuration duration;

public:
    TrafficLight(Road* road, SignalDuration duration = SignalDuration())
        : road(road), currentSignal(Signal::RED), duration(duration)
    {
    }

    void setSignal(Signal signal)
    {
        currentSignal = signal;
        cout << "[" << road->getRoadNo() << "] Signal -> " << signalToString(signal) << "\n";
        road->notifyAll(signal);
    }

    Signal getCurrentSignal() const
    {
        return currentSignal;
    }

    Road* getRoad() const
    {
        return road;
    }

    SignalDuration getDuration() const
    {
        return duration;
    }
};


// ─── Intersection (coordinator) ───────────────────────────────────────────────

class Intersection {
private:
    vector<TrafficLight*> lights;

    void setAllRed()
    {
        for (auto light : lights)
        {
            light->setSignal(Signal::RED);
        }
    }

    // Returns index of the first light whose road has an emergency vehicle, -1 if none
    int findEmergencyLightIdx() const
    {
        for (int i = 0; i < (int)lights.size(); i++)
        {
            if (lights[i]->getRoad()->hasEmergencyVehicle())
            {
                return i;
            }
        }
        return -1;
    }

    void handleEmergency(int idx)
    {
        cout << "\n*** EMERGENCY on " << lights[idx]->getRoad()->getRoadNo()
             << " — preempting all signals ***\n";

        setAllRed();

        lights[idx]->setSignal(Signal::GREEN);
        waitForSeconds(lights[idx]->getDuration().green);

        lights[idx]->setSignal(Signal::YELLOW);
        waitForSeconds(lights[idx]->getDuration().yellow);

        lights[idx]->setSignal(Signal::RED);

        cout << "*** Emergency cleared — resuming normal cycle ***\n\n";
    }

    // Give one road its full phase; all others stay RED
    void runOneTurn(int idx)
    {
        setAllRed();
        waitForSeconds(lights[idx]->getDuration().red);

        lights[idx]->setSignal(Signal::GREEN);
        waitForSeconds(lights[idx]->getDuration().green);

        lights[idx]->setSignal(Signal::YELLOW);
        waitForSeconds(lights[idx]->getDuration().yellow);
    }

public:
    explicit Intersection(vector<TrafficLight*> lights) : lights(lights)
    {
        setAllRed();
    }

    void runCycle(int rounds = 1)
    {
        for (int r = 0; r < rounds; r++)
        {
            cout << "\n=== Cycle " << (r + 1) << " ===\n";

            for (int i = 0; i < (int)lights.size(); i++)
            {
                int emergencyIdx = findEmergencyLightIdx();

                if (emergencyIdx != -1)
                {
                    handleEmergency(emergencyIdx);
                    i--;  // retry this slot after emergency is resolved
                    continue;
                }

                runOneTurn(i);
            }
        }

        setAllRed();
    }
};


// ─── Main ─────────────────────────────────────────────────────────────────────

int main()
{
    Vehicle v1(VehicleType::BIKE,   "V1");
    Vehicle v2(VehicleType::CAR,    "V2");
    Vehicle v3(VehicleType::SCOOTY, "V3");
    Vehicle v4(VehicleType::CAR,    "V4");
    Vehicle v5(VehicleType::BIKE,   "V5");

    Road r1("R1", {&v1, &v2});
    Road r2("R2", {&v3, &v4, &v5});

    // R1 is a busier road — gets a longer green phase
    TrafficLight tl1(&r1, SignalDuration(5, 2, 8));
    TrafficLight tl2(&r2, SignalDuration(5, 2, 5));

    Intersection intersection({&tl1, &tl2});

    // Normal cycle
    intersection.runCycle(1);

    // Emergency vehicle arrives on R1
    cout << "\n--- An ambulance arrives on R1 ---\n";
    Vehicle ambulance(VehicleType::AMBULANCE, "AMB-01");
    r1.addVehicle(&ambulance);

    intersection.runCycle(1);

    // Ambulance clears
    r1.removeVehicle("AMB-01");
    cout << "\n--- Ambulance has cleared R1 ---\n";

    intersection.runCycle(1);

    return 0;
}
