#include "vehicle.h"

Vehicle::Vehicle() : _id(""), _name(""), _chance(0), _status(-1), _dir(0), _fuel(-1)
{
}

Vehicle::Vehicle(QString id, QString name, int chance, int status, int dir, int fuel) : _id(id), _name(name),
    _chance(chance), _status(status), _dir(dir), _fuel(fuel)
{
}

Vehicle& Vehicle::operator =(const Vehicle & other)
{
    _id = other._id;
    _name = other._name;
    _chance = other._chance;
    _status = other._status;
    _dir = other._dir;
    _fuel = other._fuel;

    return *this;
}
