# KalaPhysics — Collider Shapes

This section describes each collider shape, what it represents geometrically,
why it exists, what data it requires, and how it is intended to be used within
the collision pipeline.

---

## BSP (Bounding Sphere)

**Description**  
A bounding sphere is a perfect sphere defined in world space. It is rotationally
invariant and mathematically minimal, making it the fastest possible bounding
volume for distance checks and early rejection. Because rotation has no effect
and the shape is intentionally coarse, BSPs are used exclusively in broadphase
where speed matters more than accuracy.

**Requirements**
- **center (vec3)** – world-space position of the sphere’s center  
- **radius (float)** – distance from the center to the surface  

**Usage characteristics**
- broadphase only  
- rotation does not matter  
- least accurate bounding volume  
- fastest performance  

---

## AABB (Axis-Aligned Bounding Box)

**Description**  
An axis-aligned bounding box is a rectangular volume aligned to the world axes.
It provides a simple and efficient way to represent spatial bounds without
rotation. As long as geometry remains roughly axis-aligned, an AABB offers
accuracy comparable to an OBB at a lower cost, making it a staple of broadphase
collision systems and spatial partitioning.

**Requirements**
- **min (vec3)** – world-space corner with the smallest X, Y, and Z values  
- **max (vec3)** – world-space corner with the largest X, Y, and Z values  

**Usage characteristics**
- broadphase only  
- fixed orientation  
- same accuracy as OBB unless the mesh rotates away from its axis-aligned shape  
- slightly slower than BSP but still very fast  

---

## OBB (Oriented Bounding Box)

**Description**  
An oriented bounding box is a box with fixed dimensions that can rotate freely in
space. By separating intrinsic shape from motion, OBBs provide a much tighter fit
than AABBs for rotating objects while remaining far cheaper than geometry-based
colliders. They are suitable for both broadphase refinement and narrowphase tests
where orientation matters.

**Requirements**
- **position (vec3 reference)** – world-space center of the box  
- **rotation (quat reference)** – world-space orientation defining local axes  
- **halfExtents (vec3)** – half-size of the box along each local axis  

**Usage characteristics**
- broadphase and narrowphase  
- fully rotatable  
- more accurate than AABB  
- slightly more expensive than AABB  

---

## BCP (Bounding Capsule, upright)

**Description**  
An upright bounding capsule is a vertical capsule aligned to the world up-axis.
Its smooth curvature avoids snagging on edges and produces stable contact normals,
making it ideal for characters and agents. Because the capsule is rotationally
invariant when upright, its collision behavior is fully defined by position,
height, and radius.

**Requirements**
- **position (vec3 reference)** – world-space center along the vertical axis  
- **height (float)** – distance between the centers of the hemispherical ends  
- **radius (float)** – radius of the cylindrical body and caps  

**Usage characteristics**
- broadphase and narrowphase  
- only height and radius are tweakable  
- more accurate than BSP, AABB, and OBB for character motion  
- moderate performance cost  

---

## KDOP (Discrete Oriented Polytope)

**Description**  
A KDOP is a convex volume defined by projecting geometry onto a fixed set of
oriented planes. By increasing the number of planes, the shape can more closely
approximate complex geometry while remaining suitable for broadphase use. KDOPs
occupy a middle ground between simple boxes and full convex hulls, trading
predictable performance for adjustable accuracy.

**Requirements**
- **vertices (span of vec3 references)** – source points projected onto planes  
- **position (vec3 reference)** – world-space location of the geometry  
- **rotation (quat reference)** – orientation used during projection  

**Usage characteristics**
- broadphase only  
- multi-plane volume  
- plane counts: 10_X, 10_Y, 10_Z, 18, or 26  
- more planes increase accuracy and cost  
- variable performance depending on plane count  

---

## BCH (Bounding Convex Hull)

**Description**  
A bounding convex hull is the exact convex hull enclosing a set of points. It
provides the most accurate convex representation of geometry, fitting tightly
around the true shape. Because of its computational cost, it is reserved for
narrowphase collision where precision is critical and performance tradeoffs are
acceptable.

**Requirements**
- **vertices (span of vec3 references)** – points defining the convex hull  
- **position (vec3 reference)** – world-space placement of the hull  
- **rotation (quat reference)** – orientation of the hull in world space  

**Usage characteristics**
- narrowphase only  
- most accurate convex representation  
- fits closely to the true mesh shape  
- very expensive  
