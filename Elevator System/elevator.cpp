#include <bits/stdc++.h>
using namespace std;

// Enums
enum class Direction {
    UP,
    DOWN,
    IDLE
};

enum class DoorState {
    OPEN,
    CLOSED
};

enum class ElevatorState {
    IDLE,
    MOVING,
    DOOR_OPEN,
    MAINTENANCE
};


// Requests 
//   1. External Request : Generated when someone presses UP or DOWN buttons (External request doesn't know destination)
//   2. Internal Request : Generated after entering elevator (Internal request doesn't care about direction)
class ExternalRequest {
    private:
        int sourceFloor;
        Direction direction;

    public:
        ExternalRequest(int floor, Direction dir)
        : sourceFloor(floor), direction(dir) {}

        int getSourceFloor() const {
            return sourceFloor;
        }

        Direction getDirection() const {
            return direction;
        }
};

class InternalRequest {
    private:
        int destinationFloor;

    public:
        InternalRequest(int floor)
        : destinationFloor(floor) {}

        int getDestinationFloor() const {
            return destinationFloor;
        }
};


// Display
class Display {
    private:
        int currentFloor;
        Direction direction;

    public:
        Display(int floor = 0 , Direction dir = Direction::IDLE)
        : currentFloor(floor) , direction(dir) {}

        void update(int floor, Direction dir) {
            currentFloor = floor;
            direction = dir;
        }

        void show() const {
            cout << "Floor : "
                << currentFloor
                << endl;

            cout << "Direction : ";

            if(direction == Direction::UP)
                cout << "UP";
            else if(direction == Direction::DOWN)
                cout << "DOWN";
            else
                cout << "IDLE";

            cout << endl;
        }
};


// Elevator
class Elevator {
    private:
        int id;
        int currentFloor;
        Direction direction;
        DoorState doorState;
        ElevatorState state;
        Display display;
        queue<int>requests;

    public:
        Elevator(int id)
        : id(id), currentFloor(0), direction(Direction::IDLE), doorState(DoorState::CLOSED), state(ElevatorState::IDLE) {}

        int getCurrentFloor() const {
            return currentFloor;
        }

        Direction getDirection() const {
            return direction;
        }

        ElevatorState getState() const {
            return state;
        }

        int getId() const {
            return id;
        }

        void addRequest(int floor) {
            requests.push(floor);
        }

        void openDoor() {
            doorState = DoorState::OPEN;
            state = ElevatorState::DOOR_OPEN;
            cout << "Elevator " << id
                 << " Door Open\n";
        }

        void closeDoor() {
            doorState = DoorState::CLOSED;
            state = ElevatorState::IDLE;
            cout << "Elevator " << id
                 << " Door Closed\n";
        }

        void showDisplay() const {
            display.show();
        }

        void moveOneFloor() {
            if(direction == Direction::UP) {
                currentFloor++;
            } else if(direction == Direction::DOWN) {
                currentFloor--;
            }

            display.update(currentFloor, direction);

            display.show();
        }

        void processNextRequest() {
            if(requests.empty()) return;

            int destination = requests.front();
            requests.pop();

            if(destination > currentFloor) {
                direction = Direction::UP;
            } else if(destination < currentFloor) {
                direction = Direction::DOWN;
            } else {
                openDoor();
                closeDoor();
                return;
            }

            state = ElevatorState::MOVING;

            while(destination != currentFloor) {
                moveOneFloor();
            }

            openDoor();
            closeDoor();

            direction = Direction::IDLE;
            state = ElevatorState::IDLE;
        }

};


// Elevator Controller - it will decide which elevator will be handling a particular request
// Algorithm - Nearest Idle Elevator

class IElevatorSelectionStrategy {
public:

    virtual Elevator* selectElevator(
        const vector<Elevator*>& elevators,
        const ExternalRequest& request
    ) = 0;

    virtual ~IElevatorSelectionStrategy() {}
};


class NearestElevatorStrategy : public IElevatorSelectionStrategy {

public:

    Elevator* selectElevator(
        const vector<Elevator*>& elevators,
        const ExternalRequest& request
    ) override {

        Elevator* bestElevator = nullptr;
        int minDistance = INT_MAX;

        for(Elevator* elevator : elevators) {

            if(elevator->getState() != ElevatorState::IDLE)
                continue;

            int distance =
                abs(
                    elevator->getCurrentFloor()
                    -
                    request.getSourceFloor()
                );

            if(distance < minDistance) {

                minDistance = distance;
                bestElevator = elevator;
            }
        }

        return bestElevator;
    }
};


class ElevatorController {

private:
    vector<Elevator*> elevators;
    IElevatorSelectionStrategy* strategy;

public:

    ElevatorController(
        const vector<Elevator*>& elevators,
        IElevatorSelectionStrategy* strategy
    )
    : elevators(elevators),
      strategy(strategy)
    {}

    void assignElevator(const ExternalRequest& request) {

        Elevator* bestElevator =
            strategy -> selectElevator(
                elevators,
                request
            );

        if(bestElevator == nullptr) {
            cout << "No Elevator Available\n";
            return;
        }

        cout << "Elevator "
             << bestElevator->getId()
             << " Assigned\n";

        bestElevator->addRequest(
            request.getSourceFloor()
        );

        bestElevator->processNextRequest();
    }
};


// Building
class Building {

private:
    int totalFloors;
    vector<Elevator*> elevators;
    ElevatorController* controller;

public:

    Building(
        int floors,
        int numberOfElevators
    )
    {
        totalFloors = floors;

        for(int i = 1 ; i <= numberOfElevators ; i++)
        {
            elevators.push_back(
                new Elevator(i)
            );
        }

        controller =
        new ElevatorController(
            elevators,
            new NearestElevatorStrategy()
        );
    }

    ElevatorController*
    getController()
    {
        return controller;
    }

    ~Building()
    {
        delete controller;

        for(auto elevator : elevators)
            delete elevator;
    }
};


int main() {

    // Building with 10 floors and 3 elevators
    Building building(10, 3);

    ElevatorController* controller = building.getController();

    // User at floor 5 presses UP
    controller->assignElevator(
        ExternalRequest(5, Direction::UP)
    );

    cout << "\n---------------------------\n";

    // User at floor 2 presses UP
    controller->assignElevator(
        ExternalRequest(2, Direction::UP)
    );

    cout << "\n---------------------------\n";

    // User at floor 8 presses DOWN
    controller->assignElevator(
        ExternalRequest(8, Direction::DOWN)
    );

    return 0;
}

