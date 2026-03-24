"""
arm_ik.py  -  Analytical IK for the 5-bar linkage robotic arm
All measurements in METRES.

Mechanism layout (arm plane, r = radial outward, z = up):
  Origin = yaw axis centre at height H_BASE above desk.

  Left  servo pivot: (X_L=+10.5mm, 0) in arm plane
  Right servo pivot: (X_R=-10.5mm, 0) in arm plane

  Left servo (theta_L, from +Z, positive = outward):
      pivot --[L1=50mm]--> P1 --[L2=170mm]--> A

  Right servo (theta_R, from +Z, positive = outward):
      pivot --[L3=170mm]--> B

  Rigid output link (250mm total):
      B ---[D_BA=50mm]--- A ---[L_TIP=200mm]--- GRIP TIP
"""

import numpy as np
from scipy.optimize import minimize

# ── CONSTANTS ─────────────────────────────────────────────────────
H_BASE  = 0.050      # yaw pivot height above desk
X_L     = +0.0105   # left  servo shaft offset
X_R     = -0.0105   # right servo shaft offset
L1      = 0.050     # left  pivot -> P1
L2      = 0.170     # P1   -> A
L3      = 0.170     # right pivot -> B
D_BA    = 0.050     # B    -> A  (along output link)
L_TIP   = 0.200     # A    -> gripper tip
LIM     = np.pi / 2 # ±90 deg hard stop

# ── HELPERS ───────────────────────────────────────────────────────

def _circle_intersect(cx1, cz1, r1, cx2, cz2, r2):
    """Returns intersection points (list of 0, 1, or 2 tuples) of two circles."""
    d = np.hypot(cx2 - cx1, cz2 - cz1)
    if d > r1 + r2 + 1e-9 or d < abs(r1 - r2) - 1e-9:
        return []
    a = (r1**2 - r2**2 + d**2) / (2 * d)
    h = np.sqrt(max(r1**2 - a**2, 0))
    ux = (cx2 - cx1) / d
    uz = (cz2 - cz1) / d
    mx = cx1 + a * ux
    mz = cz1 + a * uz
    return [
        (mx + h * (-uz), mz + h * ux),
        (mx - h * (-uz), mz - h * ux),
    ]

# ── FORWARD KINEMATICS ────────────────────────────────────────────

def fk_2d(theta_L, theta_R, elbow_up=True):
    """
    2D FK in the arm plane.
    Returns (tip_r, tip_z) or (None, None) if unreachable.
    elbow_up=True  → joint A above B (normal working pose).
    """
    # Left chain: pivot -> P1
    p1x = X_L + L1 * np.sin(theta_L)
    p1z =       L1 * np.cos(theta_L)

    # Right chain: pivot -> B
    bx = X_R + L3 * np.sin(theta_R)
    bz =       L3 * np.cos(theta_R)

    # A: intersection of circle(P1, L2) and circle(B, D_BA)
    pts = _circle_intersect(p1x, p1z, L2, bx, bz, D_BA)
    if not pts:
        return None, None

    # Choose elbow_up = A has higher z
    pts.sort(key=lambda p: p[1], reverse=elbow_up)
    Ax, Az = pts[0]

    # Output link direction B -> A, extended L_TIP past A = gripper tip
    out_r = Ax - bx
    out_z = Az - bz
    out_len = np.hypot(out_r, out_z)
    if out_len < 1e-9:
        return None, None
    ur = out_r / out_len
    uz = out_z / out_len

    return Ax + L_TIP * ur, Az + L_TIP * uz


def fk_3d(theta_yaw, theta_L, theta_R, elbow_up=True):
    """3D FK. Returns world [x, y, z] or None."""
    tip_r, tip_z = fk_2d(theta_L, theta_R, elbow_up)
    if tip_r is None:
        return None
    return np.array([
        tip_r * np.cos(theta_yaw),
        tip_r * np.sin(theta_yaw),
        H_BASE + tip_z,
    ])


# ── INVERSE KINEMATICS ────────────────────────────────────────────

def ik(target_xyz, elbow_up=True):
    """
    Solve IK for target [x, y, z] in world frame (metres).

    Returns:
        theta_yaw  (rad)
        theta_L    (rad)  left servo
        theta_R    (rad)  right servo
        error_mm   (float) position error in mm
        success    (bool)  True if error < 5 mm and all angles within ±90 deg
    """
    x, y, z = float(target_xyz[0]), float(target_xyz[1]), float(target_xyz[2])

    # ── Yaw: trivial from horizontal angle ──────────────────────
    theta_yaw = np.arctan2(y, x)
    r_target  = np.hypot(x, y)
    z_arm     = z - H_BASE

    if abs(theta_yaw) > LIM:
        return None, None, None, float('nan'), False

    # ── 5-bar solve: bounded optimisation ───────────────────────
    def cost(angles):
        tL, tR = angles
        tr, tz = fk_2d(tL, tR, elbow_up)
        if tr is None:
            return 1e6
        return (tr - r_target)**2 + (tz - z_arm)**2

    bounds = [(-LIM, LIM), (-LIM, LIM)]

    best_sol = None
    best_val = np.inf

    # Multi-start grid within ±90 deg
    for a_deg in np.linspace(-80, 80, 7):
        for b_deg in np.linspace(-80, 80, 7):
            x0 = [np.deg2rad(a_deg), np.deg2rad(b_deg)]
            try:
                res = minimize(cost, x0, method='L-BFGS-B', bounds=bounds,
                               options={'ftol': 1e-14, 'gtol': 1e-10, 'maxiter': 500})
                if res.fun < best_val:
                    best_val = res.fun
                    best_sol = res.x
            except Exception:
                continue

    if best_sol is None:
        return None, None, None, float('nan'), False

    theta_L, theta_R = best_sol

    # ── Final error via FK ───────────────────────────────────────
    tip = fk_3d(theta_yaw, theta_L, theta_R, elbow_up)
    if tip is None:
        return None, None, None, float('nan'), False

    err_mm = np.linalg.norm(tip - np.array([x, y, z])) * 1000.0
    success = (err_mm < 5.0
               and abs(theta_L) <= LIM
               and abs(theta_R) <= LIM)

    return theta_yaw, theta_L, theta_R, err_mm, success


# ── MAIN DEMO ─────────────────────────────────────────────────────

if __name__ == "__main__":
    print("=" * 62)
    print("  5-Bar Linkage Arm  -  Analytical IK Solver")
    print("=" * 62)

    test_targets = [[0.20,  0.00,  0.20]]

    print("\nIK Results:")
    for target in test_targets:
        yaw, tL, tR, err_mm, ok = ik(target)
        print(f"\n  Target : X={target[0]:.3f}  Y={target[1]:.3f}  Z={target[2]:.3f} m")
        if ok:
            tip = fk_3d(yaw, tL, tR)
            print(f"    yaw   = {np.rad2deg(yaw):+7.2f} deg")
            print(f"    left  = {np.rad2deg(tL):+7.2f} deg")
            print(f"    right = {np.rad2deg(tR):+7.2f} deg")
            print(f"    FK:    ({tip[0]:.4f}, {tip[1]:.4f}, {tip[2]:.4f})")
            print(f"    Error: {err_mm:.3f} mm  [OK]")
        else:
            tip_r, tip_z = fk_2d(tL or 0, tR or 0)
            tip_3d = fk_3d(yaw or 0, tL or 0, tR or 0) if tL is not None else None
            print(f"    [FAIL] err={err_mm:.2f} mm  Outside joint limits or unreachable")

    print("\n" + "=" * 62)
