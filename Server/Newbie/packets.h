#pragma once
#pragma pack(push, 1)
namespace packets
{
	// 캐릭터 물리 정보
	struct characterPhysInfo
	{
		struct Location
		{
			/** Vector's X component. */
			float X;

			/** Vector's Y component. */
			float Y;

			/** Vector's Z component. */
			float Z;
		} Location;
		struct Rotation
		{
			/** Rotation around the right axis (around Y axis), Looking up and down (0=Straight Ahead, +Up, -Down) */
			float Pitch;

			/** Rotation around the up axis (around Z axis), Running in circles 0=East, +North, -South. */
			float Yaw;

			/** Rotation around the forward axis (around X axis), Tilting your head, 0=Straight, +Clockwise, -CCW. */
			float Roll;
		} Rotation;
		struct Velocity
		{
			/** Vector's X component. */
			float X;

			/** Vector's Y component. */
			float Y;

			/** Vector's Z component. */
			float Z;
		} Velocity;
	} characterPhysInfo;
}
#pragma pack(pop)