#include "buildingmodel.h"
#include <QBrush>
#include <QVector>
#include <QDebug>
#include <QDateTime>
#include "features.h"

BuildingModel::BuildingModel(bool active[][9], QObject *parent) :
    QAbstractTableModel(parent), _rows(0), _cols(0), _z(0), _maxX(0), _maxY(0)
{
    int maxX = 0;
    int maxY = 0;
    for (int row = 0; row < 9; row++)
    {
        for (int col = 0; col < 9; col++)
        {
            _omtv.append(new OvermapTerrain(active[row][col]));

            if (active[row][col])
            {
                maxX = qMax(maxX, col+1);
                maxY = qMax(maxY, row+1);
            }
        }
    }

    _rows = maxY * OVERMAP_TERRAIN_WIDTH;
    _cols = maxX * OVERMAP_TERRAIN_WIDTH;

    _maxY = maxY;
    _maxX = maxX;
}

BuildingModel::BuildingModel(QVector<bool> active, QObject *parent) :
    QAbstractTableModel(parent), _rows(0), _cols(0), _z(0), _maxX(0), _maxY(0)
{
    int maxX = 0;
    int maxY = 0;
    for (int z = 10; z >= -10; z--)
    {
        for (int row = 0; row < 9; row++)
        {
            for (int col = 0; col < 9; col++)
            {
                int index = ((10 - z) * 9 * 9) + (row * 9) + col;
                _omtv.append(new OvermapTerrain(active[index]));

                if (active[index])
                {
                    maxX = qMax(maxX, col+1);
                    maxY = qMax(maxY, row+1);
                }
            }
        }
    }

    _rows = maxY * OVERMAP_TERRAIN_WIDTH;
    _cols = maxX * OVERMAP_TERRAIN_WIDTH;

    _maxY = maxY;
    _maxX = maxX;
}

BuildingModel::~BuildingModel()
{
    foreach (OvermapTerrain* omt, _omtv)
    {
        delete omt;
    }
}

int BuildingModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _rows;
}

int BuildingModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _cols;
}

QVariant BuildingModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    Tripoint tp = GetTileIndex(index);
    OvermapTerrain* omt = _omtv[OMTvIndex(index)];

    // output if we have inactive tiles for disjointed specials, ex: O  O
    if (!omt->IsActive())
    {
        switch(role)
        {
        case Qt::DisplayRole:
            return QChar(' ');
        case Qt::BackgroundRole:
            return QBrush(Qt::gray);
        case Qt::ForegroundRole:
            return QBrush(Qt::gray);
        default:
            return QVariant();
        }
    }

    Tile tile = omt->GetTile(tp);

    switch(role)
    {
    case Qt::DisplayRole:
    {
        if (tile.IsLineDrawing())
        {
            return GetLineDrawingChar(index);
        }
        else if (tile.GetTerrainID() == "t_null")
        {
            // grab tile from z level below then above. look for up/down connections
            Tile below = _omtv[Index(index, _z - 1)]->GetTile(GetTileIndex(index));
            if (Features::GetTerrain(below.GetTerrainID()).HasFlag("GOES_UP"))
            {
                return below.GetTerrainChar();
            }
            Tile above = _omtv[Index(index, _z + 1)]->GetTile(GetTileIndex(index));
            if (Features::GetTerrain(above.GetTerrainID()).HasFlag("GOES_DOWN"))
            {
                return above.GetTerrainChar();
            }
            return tile.GetDisplayChar();
        }
        else
        {
            return tile.GetDisplayChar();
        }
    }
    case Qt::BackgroundRole:
    {
        if (tile.GetTerrainID() == "t_null")
        {
            Tile below = _omtv[Index(index, _z - 1)]->GetTile(GetTileIndex(index));
            if (Features::GetTerrain(below.GetTerrainID()).HasFlag("GOES_UP"))
            {
                return QBrush(Qt::gray);
            }
            Tile above = _omtv[Index(index, _z + 1)]->GetTile(GetTileIndex(index));
            if (Features::GetTerrain(above.GetTerrainID()).HasFlag("GOES_DOWN"))
            {
                return QBrush(Qt::gray);
            }
        }
        return QBrush(tile.GetBackgroundColor());
    }
    case Qt::ForegroundRole:
    {
        if (tile.GetTerrainID() == "t_null")
        {
            Tile below = _omtv[Index(index, _z - 1)]->GetTile(GetTileIndex(index));
            if (Features::GetTerrain(below.GetTerrainID()).HasFlag("GOES_UP"))
            {
                return QBrush(Features::GetTerrain(below.GetTerrainID()).GetForeground());
            }
            Tile above = _omtv[Index(index, _z + 1)]->GetTile(GetTileIndex(index));
            if (Features::GetTerrain(above.GetTerrainID()).HasFlag("GOES_DOWN"))
            {
                return QBrush(Features::GetTerrain(above.GetTerrainID()).GetForeground());
            }
        }
        return QBrush(tile.GetForegroundColor());
    }
    default:
        return QVariant();
    }

    return QVariant();
}

bool BuildingModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    OvermapTerrain* omt = _omtv[OMTvIndex(index)];
    if (!omt->IsActive())
    {
        return false;
    }
    Tile& t = omt->GetTile(GetTileIndex(index));
    switch(role)
    {
    case TerrainRole:
        t.SetTerrain(value.toString());
        emit dataChanged(index, index);
        return true;
    case FurnitureRole:
        t.SetFurniture(value.toString());
        emit dataChanged(index, index);
        return true;
    case TrapRole:
        t.SetTrap(value.toString());
        emit dataChanged(index, index);
        return true;
    case MonsterGroupRole:
        t.SetMonsterGroup(value.value<MonsterGroup>());
        emit dataChanged(index, index);
        return true;
    case ItemGroupRole:
        t.SetItemGroup(value.value<ItemGroup>());
        emit dataChanged(index, index);
    case MonsterRole:
        t.SetMonster(value.toString());
        emit dataChanged(index, index);
        return true;
    case ItemRole:
        t.AddItem(value.toString());
        emit dataChanged(index, index);
        return true;
    case VehicleRole:
        t.SetVehicle(value.value<Vehicle>());
        emit dataChanged(index, index);
        return true;
    case ToiletRole:
        t.SetToilet(value.toBool());
        emit dataChanged(index, index);
        return true;
    case NpcRole:
        t.SetNPC(value.toString());
        emit dataChanged(index, index);
        break;
    case SignRole:
        t.SetSignage(value.toString());
        emit dataChanged(index, index);
        break;
    case RadiationRole:
        t.SetRadiation(value.toInt());
        emit dataChanged(index, index);
        break;
    case VendingRole:
        t.SetVending(value.toString());
        emit dataChanged(index, index);
        break;
    case GasPumpRole:
        t.SetGasPump(value.value<GasPump>());
        emit dataChanged(index, index);
        break;
    case RubbleRole:
        t.SetRubble(value.value<Rubble>());
        emit dataChanged(index, index);
        break;
    case FieldRole:
        t.SetField(value.value<Field>());
        emit dataChanged(index, index);
        break;
    default:
        return false;
    }
    return false;
}

void BuildingModel::Erase(const QModelIndex& index, int role)
{
    OvermapTerrain* omt = _omtv[OMTvIndex(index)];
    if (!omt->IsActive())
    {
        return;
    }
    Tile& t = omt->GetTile(GetTileIndex(index));

    switch(role)
    {
    case TerrainRole:
        t.SetTerrain(null_terrain.GetID());
        break;
    case FurnitureRole:
        t.SetFurniture(null_furniture.GetID());
        break;
    case TrapRole:
        t.SetTrap(null_trap.GetID());
        break;
    case MonsterGroupRole:
        t.SetMonsterGroup(null_monster_group);
        break;
    case ItemGroupRole:
        t.SetItemGroup(null_item_group);
        break;
    case MonsterRole:
        t.SetMonster("");
        break;
    case ItemRole:
        t.AddItem("");
        break;
    case VehicleRole:
        t.SetVehicle(null_vehicle);
        break;
    case ToiletRole:
        t.SetToilet(false);
        break;
    case NpcRole:
        t.SetNPC("");
        break;
    case SignRole:
        t.SetSignage("");
        break;
    case RadiationRole:
        t.SetRadiation(0);
        break;
    case VendingRole:
        t.SetVending("");
        break;
    case GasPumpRole:
        t.SetGasPump(null_gas_pump);
        break;
    case RubbleRole:
        t.SetRubble(null_rubble);
        break;
    case FieldRole:
        t.SetField(null_field);
        break;
    default:
        return;
    }
    emit dataChanged(index, index);
}

QList<OvermapTerrain*> BuildingModel::GetOvermapTerrains()
{
    return _omtv;
}

QList<OvermapTerrain*> BuildingModel::GetActiveOvermapTerrains()
{
    QList<OvermapTerrain*> omts;

    foreach (OvermapTerrain* omt, _omtv)
    {
        if (omt->IsActive())
        {
            omts.append(omt);
        }
    }

    return omts;
}

// TODO: i dont like these, but i want something similar in the end
BuildingModel* BuildingModel::CreateSpecialModel(OvermapSpecialData data)
{
    QVector<bool> activeList;
    foreach (OMTData d, data.GetOMTData())
    {
        if (d.GetName() != "")
        {
            activeList.append(true);
        }
        else
        {
            activeList.append(false);
        }
    }
    BuildingModel* model = new BuildingModel(activeList);

    for (int i = 0; i < model->GetOvermapTerrains().count(); i++)
    {
        model->GetOvermapTerrains()[i]->SetData(data.GetOMTData()[i]);
    }
    return model;
}

BuildingModel* BuildingModel::CreateNormalModel(OMTData data)
{
    QVector<bool> activeList;
    activeList.fill(false, 21 * 9 * 9);
    activeList.replace(10*9*9, true);
    BuildingModel* model = new BuildingModel(activeList);
    model->GetActiveOvermapTerrains()[0]->SetData(data);

    return model;
}

void BuildingModel::OnOmtLoaded(OvermapTerrain* omt)
{
    for (int row = 0; row < OVERMAP_TERRAIN_WIDTH; row++)
    {
        QString rowString = "";
        for (int col = 0; col < OVERMAP_TERRAIN_WIDTH; col++)
        {
            Tripoint p(col, row, 0);
            rowString.append(omt->GetTile(p).GetDisplayChar());
        }
        qDebug() << rowString.replace(QChar(0x253C), "|");
    }
    delete _omtv[0]; // dont want to leave that little bugger hanging, i think...
    _omtv[0] = omt;

    emit dataChanged(this->index(0, 0), this->index(23, 23));
}

void BuildingModel::OnSelectedIndex(QModelIndex index)
{
    // find tile at index, send it to the editor
    Tile& t = GetTileFromIndex(index);
    emit TileSelected(t);
}

void BuildingModel::OnEraseIndex(QModelIndex index)
{
    qDebug() << "Erasing";
    Tile& t = GetTileFromIndex(index);
    t.EraseAll();
    emit dataChanged(index, index);
}

void BuildingModel::OnZLevelChanged(int level)
{
    _z = level;
    emit dataChanged(index(0, 0), index(_maxY, _maxX));
}

OvermapTerrain* BuildingModel::GetOMTFromIndex(const QModelIndex & index) const
{
    return _omtv[OMTvIndex(index)];
}

Tripoint BuildingModel::GetTileIndex(const QModelIndex& index) const
{
    return GetTileIndex(index.row(), index.column());
}

Tripoint BuildingModel::GetTileIndex(int row, int column) const
{
    int x = column % OVERMAP_TERRAIN_WIDTH;
    int y = row % OVERMAP_TERRAIN_WIDTH;
    int z = 0;

    return Tripoint(x, y, z);
}

Tile& BuildingModel::GetTileFromIndex(const QModelIndex & index) const
{
    return GetOMTFromIndex(index)->GetTile(GetTileIndex(index));
}

QChar BuildingModel::GetLineDrawingChar(const QModelIndex & index) const
{
    // NS, EW, NE, NW, ES, SW, NES, NSW, NEW, ESW, NESW
    static QList<QChar> lineDrawingChars = { 0x2502, 0x2500, 0x2514, 0x2518, 0x250C, 0x2510, 0x251C, 0x2524, 0x2534, 0x252C, 0x253C };

    int neighbors = 0;
    QModelIndex adjacent = this->index(index.row(), index.column()+1);
    if (adjacent.isValid())
    {
        QString terrain = GetTileFromIndex(adjacent).GetTerrainID();
        if (Features::GetTerrain(terrain).HasFlag("CONNECT_TO_WALL") || Features::GetTerrain(terrain).HasFlag("AUTO_WALL_SYMBOL"))
        {
            neighbors |= EAST;
        }
    }

    adjacent = this->index(index.row(), index.column()-1);
    if (adjacent.isValid())
    {
        QString terrain = GetTileFromIndex(adjacent).GetTerrainID();
        if (Features::GetTerrain(terrain).HasFlag("CONNECT_TO_WALL") || Features::GetTerrain(terrain).HasFlag("AUTO_WALL_SYMBOL"))
        {
            neighbors |= WEST;
        }
    }

    adjacent = this->index(index.row()-1, index.column());
    if (adjacent.isValid())
    {
        QString terrain = GetTileFromIndex(adjacent).GetTerrainID();
        if (Features::GetTerrain(terrain).HasFlag("CONNECT_TO_WALL") || Features::GetTerrain(terrain).HasFlag("AUTO_WALL_SYMBOL"))
        {
            neighbors |= NORTH;
        }
    }

    adjacent = this->index(index.row()+1, index.column());
    if (adjacent.isValid())
    {
        QString terrain = GetTileFromIndex(adjacent).GetTerrainID();
        if (Features::GetTerrain(terrain).HasFlag("CONNECT_TO_WALL") || Features::GetTerrain(terrain).HasFlag("AUTO_WALL_SYMBOL"))
        {
            neighbors |= SOUTH;
        }
    }

    switch(neighbors)
    {
    case 0:
    case (NORTH | EAST | SOUTH | WEST):
        return lineDrawingChars[NESW];
    case (NORTH):
    case (SOUTH):
    case (NORTH | SOUTH):
        return lineDrawingChars[NS];
    case (EAST):
    case (WEST):
    case (EAST | WEST):
        return lineDrawingChars[EW];
    case (NORTH | EAST):
        return lineDrawingChars[NE];
    case (SOUTH | EAST):
        return lineDrawingChars[SE];
    case (NORTH | EAST | SOUTH):
        return lineDrawingChars[NES];
    case (NORTH | WEST):
        return lineDrawingChars[NW];
    case (NORTH | EAST | WEST):
        return lineDrawingChars[NEW];
    case (SOUTH | WEST):
        return lineDrawingChars[SW];
    case (NORTH | SOUTH | WEST):
        return lineDrawingChars[NSW];
    case (EAST | SOUTH | WEST):
        return lineDrawingChars[ESW];
    default:
        return lineDrawingChars[NESW];
    }
}

int BuildingModel::OMTvIndex(const QModelIndex &index) const
{
    //return (((index.row() / OVERMAP_TERRAIN_WIDTH) * 9) + (index.column() / OVERMAP_TERRAIN_WIDTH));

    int zComponent = ((10 - _z) * 9 * 9);
    int rowComponent = ((index.row() / OVERMAP_TERRAIN_WIDTH) * 9);
    int colComponent = (index.column() / OVERMAP_TERRAIN_WIDTH);
    return zComponent + rowComponent + colComponent;
}

int BuildingModel::Index(Tripoint p) const
{
    return Index(p.x(), p.y(), p.z());
}

int BuildingModel::Index(int x, int y, int z) const
{
    return ((10 - z) * 9 * 9) + ((y / OVERMAP_TERRAIN_WIDTH) * 9) + (x / OVERMAP_TERRAIN_WIDTH);
}

int BuildingModel::Index(const QModelIndex &index) const
{
    return OMTvIndex(index);
}

int BuildingModel::Index(const QModelIndex &index, int z) const
{
    return Index(index.column(), index.row(), z);
}
