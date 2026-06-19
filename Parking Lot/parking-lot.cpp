#include <bits/stdc++.h>
using namespace std;

enum class VehicleType {
    SCOOTY,
    BIKE,
    CAR
};

class Vehicle {
    private:
        VehicleType type;
        string licensePlate;

    public:
        Vehicle(VehicleType type, string licensePlate) {
            this -> type = type;
            this -> licensePlate = licensePlate;
        }

        VehicleType getType() { 
            return type; 
        }

        string getLicensePlate() { 
            return licensePlate; 
        }
};


class ParkingSpot {
    private:
        int spotId;
        VehicleType allowedType;
        bool isOccupied;
        Vehicle* parkedVehicle;

    public:
        ParkingSpot(int spotId , VehicleType allowedType) {
            this -> spotId = spotId;
            this -> allowedType = allowedType;
            this -> isOccupied = false;
            this -> parkedVehicle = nullptr;
        }

        bool canFit(Vehicle* vehicle) {
            return !isOccupied && allowedType == vehicle -> getType();
        }

        void park(Vehicle* vehicle) {
            parkedVehicle = vehicle;
            isOccupied = true;
        }

        void release() {
            parkedVehicle = nullptr;
            isOccupied = false;
        }

        int getSpotId() {
            return spotId;
        }

        bool getIsOccupied() {
            return isOccupied;
        }

        VehicleType getAllowedType() {
            return allowedType;
        }

};


class Level {
    private:
        int levelId;
        vector<ParkingSpot*>spots;

    public:
        Level(int levelId , int numScooty , int numBikes , int numCars) {
            this -> levelId = levelId;
            int spotId  = 0;

            for(int i = 0 ; i < numScooty ; i++) {
                spots.push_back(new ParkingSpot(spotId++ , VehicleType::SCOOTY));
            }

            for(int i = 0 ; i < numBikes ; i++) {
                spots.push_back(new ParkingSpot(spotId++ , VehicleType::BIKE));
            }

            for(int i = 0 ; i < numCars ; i++) {
                spots.push_back(new ParkingSpot(spotId++ , VehicleType::CAR));
            }
        }

        ParkingSpot* parkVehicle(Vehicle* vehicle) {
            for(auto spot : spots) {
                if(spot -> canFit(vehicle)) {
                    spot -> park(vehicle);
                    return spot;
                }
            }

            return nullptr;
        }

        bool releaseSpot(int spotId) {
            for(auto spot : spots) {
                if(spot -> getSpotId() == spotId) {
                    spot -> release();
                    return true;
                }
            }

            return false;
        }

        void displayStatus() {
            cout << "  Level " << levelId << ":\n";
            for (auto spot : spots) {
                cout << "    Spot " << spot -> getSpotId()
                     << " [" << (spot -> getIsOccupied() ? "X" : "O") << "]\n";
            }
        }

        ~Level() {
            for (auto spot : spots) delete spot;
        }
};



class ParkingLot {
    private:
        vector<Level*>levels;
        map<string , pair<int , int>>vehicleMap;    // licensePlate -> {levelIdx, spotId}
        static ParkingLot* instance;

        ParkingLot() {}

    public:
        static ParkingLot* getInstance() {
            if(!instance) instance = new ParkingLot();
            return nullptr;
        }

        void addLevel(int numScooty, int numBike, int numCar) {
            levels.push_back(new Level(levels.size(), numScooty, numBike, numCar));
        }

        bool entry(Vehicle* vehicle) {
            for (int i = 0; i < levels.size(); i++) {
                ParkingSpot* spot = levels[i] -> parkVehicle(vehicle);
                if (spot) {
                    vehicleMap[vehicle -> getLicensePlate()] = {i, spot -> getSpotId()};
                    cout << "Vehicle " << vehicle -> getLicensePlate()
                         << " parked at Level " << i
                         << ", Spot " << spot -> getSpotId() << "\n";
                    return true;
                }
            }
            cout << "No spot available for " << vehicle -> getLicensePlate() << "\n";
            return false;
        }

        bool exit(Vehicle* vehicle) {
            string plate = vehicle -> getLicensePlate();
            if (vehicleMap.find(plate) == vehicleMap.end()) {
                cout << "Vehicle " << plate << " not found\n";
                return false;
            }

            auto [levelIdx, spotId] = vehicleMap[plate];
            levels[levelIdx] -> releaseSpot(spotId);

            vehicleMap.erase(plate);

            cout << "Vehicle " << plate << " exited from Level "
                 << levelIdx << ", Spot " << spotId << "\n";
            return true;
        }

        void displayStatus() {
            cout << "=== Parking Lot Status ===\n";
            for (auto level : levels) level -> displayStatus();
        }

        ~ParkingLot() {
            for (auto level : levels) delete level;
        }
};

ParkingLot* ParkingLot::instance = nullptr;


int main() {

    ParkingLot* lot = ParkingLot::getInstance();
    lot -> addLevel(2, 2, 2); // Level 0: 2 scooty, 2 bike, 2 car spots
    lot -> addLevel(1, 1, 3); // Level 1: 1 scooty, 1 bike, 3 car spots

    Vehicle* v1 = new Vehicle(VehicleType::CAR, "DL-01-AA-1111");
    Vehicle* v2 = new Vehicle(VehicleType::BIKE, "DL-02-BB-2222");
    Vehicle* v3 = new Vehicle(VehicleType::SCOOTY, "DL-03-CC-3333");
    Vehicle* v4 = new Vehicle(VehicleType::CAR, "DL-04-DD-4444");

    lot -> entry(v1);
    lot -> entry(v2);
    lot -> entry(v3);
    lot -> entry(v4);

    lot -> displayStatus();

    lot -> exit(v1);
    lot -> displayStatus();

    delete v1; delete v2; delete v3; delete v4;
    return 0;
}
