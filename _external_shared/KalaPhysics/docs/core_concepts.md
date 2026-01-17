# KalaPhysics — Core Concepts and Terminology

This document defines the core terminology and conceptual building blocks used throughout KalaPhysics. It establishes a shared vocabulary and mental model for geometry, rays, and collision processing, providing the conceptual foundation on which the collision system is built.

---

## Custom terminology used in this document

### Ambidirectional
- unconstrained direction
- could be one or multiple directions
- could be opposite directions, or any direction at all or directions change over time
- independent of object motion

### Instant raycasting
- happens at the current frame, no delay factors
- doesnt care if the target mesh is static or in motion

### Delayed raycasting
- the ray is affected by one or more factors and takes time to reach its target
- splits into static and dynamic mesh collision

### Half mesh
- volumetric geometry that does not enclose a full 3D volume
- does not have an "interior"

**Disqualifiers (not a true full mesh):**
- one or more faces missing – not truly enclosed
- edges poking through faces
- faces overlapping at non-manifold angles
- twisted or inverted geometry
- vertices welded incorrectly causing face inversion

**Examples:**
- line segments
- triangles
- quads
- ribbon or strip meshes

### Full mesh
- a closed volumetric 3D geometry with no missing faces
- a full mesh can be convex or concave as long as it encloses a fully connected mesh
- separates interior from exterior

**Examples:**
- cubes
- spheres
- convex hulls
- bounding volumes

---

## Ray–object intersections

### Bounding
- ray vs BSP (bounding sphere)
- ray vs AABB
- ray vs OBB
- ray vs BCP (bounding capsule)
- ray vs KDOP
- ray vs BCH (bounding convex hull)

### Half mesh
- ray vs line (edge between two points)
- ray vs line segment (connection of lines)
- ray vs triangle (three points that form a triangle and have a filled surface)
- ray vs quad (four points that form a face and have a filled surface)

### Full mesh
- ray vs mesh (regular user-submitted mesh of any kind, via triangle batches or BVH / AABB trees)
- ray vs heightmap (raw data of a heightmap or the heightmap picture pixels themselves)
- ray vs terrain (real terrain mesh, unlike a heightmap)

### Instant ray
- no lifetime, happens instantly at the current frame
- straight or curved
- no velocity, no acceleration

### Delayed ray
- lives until collision or for how long the lifetime has been set
- can be given a speed
- can accelerate and decelerate over time
- can be given a curve unaffected by gravity, weight, wind
- can be affected by gravity
- can be affected by weight (downward force multiplier affected by gravity strength)
- can be affected by wind (ambidirectional force multiplier)
- can be affected by drag (opposes motion against moving direction)
- can be affected by friction (opposes motion against moving direction along surfaces (surface-bound drag))

### Delayed ray hit target type matters
- **static target** – only ray has a trajectory, only track the position of the ray over time, mesh stays fixed in space
- **moving target** – both ray and mesh have a trajectory, also track the position of the mesh over time
- **swept target** – both ray and mesh have a trajectory, also track the entire space the mesh occupies over time

---

## Broadphase collision

Find potential collision pairs without doing real collision math,  
which objects are close enough that we should bother testing them for real.

### Broadphase
- cheap, inaccurate, detects overlap regions
- runs on thousands of objects
- often runs every frame
- drops more than 95% of potential pairs to be ignored

### Real world use cases
- large multiplayer open worlds
- projectile collision filtering
- culling unnecessary collision checks
- early rejection in raycasts

### Algorithms
- BVH (bounding volume hierarchy) tree construction
- quadtrees / octrees
- uniform grids
- sweep and prune
- spatial hashing (hash grid broadphase)
- layer-based broadphase (collision masks, layers, groups, filters)
- cell-based broadphase
- hybrid broadphase (use two or more systems broadphase together)

---

## Narrowphase collision

Compute the actual collision exactly between objects identified by broadphase.

### Narrowphase
- slow, expensive, high precision collision
- runs only on filtered pairs
- determines physical response
- often uses real geometry

### Real world use cases
- bullet exact impacts
- rigidbody stacking
- vehicle collision
- detailed world geometry
- melee hitboxes

### Algorithms
- penetration depth
- convex vs convex (GJK, EPA, SAT)
- distance between convex shapes (GJK)
- separating axis theorem (pure collision detection)
- triangle mesh tests (triangle-level detection)
- segment intersection
- line–quad intersections
- CCD sweeping (narrowphase contact generation)
- barycentric coordinates, triangle area
- surface normals (narrowphase returns normals to the solver)

---

## Post-narrowphase helpers

The following should *always* be handled after narrowphase collision.

### Collision response / impulse solve
- impulses
- bounces (restitution)
- friction
- constraints (ragdolls, hinge, slider, 6-DOF etc)
- constraint limits and motors
- contact softness / stiffness
- resting contact stabilization
- momentum
- mass
- inertia

### Damping
- linear and angular damping
- drag
- stabilizing large impulses
- smoothing jitter
- reducing simulation energy buildup
- micro-damping (prevent jitter in very small movements)
- sleep-threshold softening (stop forever-jitter near the sleep threshold)

### Stability / safety systems
- penetration correction
- prevent joint / constraint drift
- cap forces / velocities
- numerical safety clamps
- positional stabilization
- CCD fixup

### Integration
- linear and angular velocity
- updating position
- updating orientation
