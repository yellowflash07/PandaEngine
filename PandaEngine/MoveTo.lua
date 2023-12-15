-- Assume you have already created 'moveTo' in C++ and exposed it to Lua

-- Set parameters for MoveTo
moveTo:SetParams(mesh, sphere.drawPosition, 20.5)

-- Execute the MoveTo command
moveTo:Execute(deltaTime)