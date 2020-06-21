#ifndef ROOM_H
#define ROOM_H


class Room {
public:
    // Constructor
    Room(int _id, int _capacity, int _penalty)
        : id(_id), capacity(_capacity), penalty(_penalty), numExamsScheduled(0), numOccupiedSeats(0) { }

    // Public interface
    int getId() const { return id; }
    int getCapacity() const { return capacity; }
    int getPenalty() const { return penalty; }

    int getNumExamsScheduled() const     { return numExamsScheduled;  }
    void setNumExamsScheduled(int value) { numExamsScheduled = value; }

    int getNumOccupiedSeats() const     { return numOccupiedSeats;  }
    void setNumOccupiedSeats(int value) { numOccupiedSeats = value; }


private:
    // Private fields

    int id; // Room id
    int capacity; // Room capacity
    int penalty; // Room penalty
    int numExamsScheduled; // Number of exams scheduled in this room
    int numOccupiedSeats;
};


#endif // ROOM_H

