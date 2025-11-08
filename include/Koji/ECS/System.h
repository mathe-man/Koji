#pragma once

class System {
public:
    virtual ~System() = default;
    virtual bool Init()     { return true; }
    virtual bool Update()   { return true; }
    virtual bool Close()   { return true; }
};
