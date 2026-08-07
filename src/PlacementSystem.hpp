#pragma once
#include <string>
#include <vector>

enum class PlacementCategory {
    Wall,
    Prop,
    Tile
};

struct PlacedObject {
    std::string id;
    PlacementCategory category = PlacementCategory::Wall;
    int x = 0;
    int y = 0;
    int height = 1;
    int layer = 0;
    float rotation = 0.0f;
    std::string material;
    std::vector<std::string> mods;
    int connectionMask = 0;
};

class PlacementSystem {
public:
    static constexpr int NORTH = 1;
    static constexpr int SOUTH = 2;
    static constexpr int EAST = 4;
    static constexpr int WEST = 8;

    PlacementSystem();

    void PlaceWall(int x, int y, const std::string& material, int height, int layer,
                   const std::vector<std::string>& mods);
    void PlaceProp(int x, int y, const std::string& id, int layer, float rotation);
    void PlaceTile(int x, int y, const std::string& tileType, int layer);

    std::string GetWallTexture(const PlacedObject& wall) const;
    std::string GetPropTexture(const PlacedObject& prop) const;

    void SnapToGrid(int& x, int& y);
    void SnapToWall(int x, int y, int& outX, int& outY);
    void UpdateWallConnections(int x, int y);

    PlacedObject* GetObjectAt(int x, int y, int layer);
    const std::vector<PlacedObject>& GetAllObjects() const;
    void RemoveObject(int x, int y, int layer);

    bool SaveToFile(const std::string& filename);
    bool LoadFromFile(const std::string& filename);

private:
    std::vector<PlacedObject> objects;
};
