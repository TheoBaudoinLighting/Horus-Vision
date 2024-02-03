# Camera Module for Horus Vision

This directory contains the core components of the camera system for Horus Vision, suitable for both OpenGL and Radeon rendering environments.

## Files Overview

- `hrs_opengl_camera.cpp` & `hrs_opengl_camera.h`: Implements the `HorusOpenGLCamera` class for OpenGL rendering.
- `hrs_radeon_camera.cpp` & `hrs_radeon_camera.h`: Implements the `HorusRadeonCamera` class for Radeon-based rendering.

## Usage

1. Include the appropriate header file in your project.
2. Instantiate a camera object (`HorusOpenGLCamera` or `HorusRadeonCamera`) as per your rendering environment.
3. Initialize and update the camera using its methods.

## Integration

- **OpenGL Projects**: Use `HorusOpenGLCamera` for comprehensive camera management.
- **Radeon Projects**: `HorusRadeonCamera` is optimized for Radeon's rendering capabilities.

## Support

For assistance or to report issues, please contact the project maintainer.

