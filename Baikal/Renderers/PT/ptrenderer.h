/**********************************************************************
Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
********************************************************************/
#pragma once

#include "math/int2.h"
#include "Core/renderer.h"
#include "CLW/clwscene.h"
#include "CLW/clw_scene_controller.h"

#include "CLW.h"


namespace Baikal
{
    class ClwOutput;
    struct ClwScene;
    class SceneTracker;

    ///< Renderer implementation
    class PtRenderer : public Renderer
    {
    public:
        // Constructor
        PtRenderer(CLWContext context, int devidx, int num_bounces);
        // Destructor
        ~PtRenderer();

        // Renderer overrides
        // Create output
        Output* CreateOutput(std::uint32_t w, std::uint32_t h) const override;
        // Delete output
        void DeleteOutput(Output* output) const override;
        // Clear output
        void Clear(RadeonRays::float3 const& val, Output& output) const override;
        // Render the scene into the output
        void Render(Scene1 const& scene) override;
        // Render single tile
        void RenderTile(Scene1 const& scene, RadeonRays::int2 const& tile_origin, RadeonRays::int2 const& tile_size) override;
        // Set output
        void SetOutput(OutputType type, Output* output) override;
        // Set number of light bounces
        void SetNumBounces(int num_bounces);
        // Interop function
        CLWKernel GetCopyKernel();
        // Add function
        CLWKernel GetAccumulateKernel();
        // Run render benchmark
        void RunBenchmark(Scene1 const& scene, std::uint32_t num_passes, BenchmarkStats& stats) override;

    protected:
        // Resize output-dependent buffers
        void ResizeWorkingSet(Output const& output);
        // Generate rays
        void GeneratePrimaryRays(ClwScene const& scene, Output const& output, int2 const& tile_size);
        // Shade first hit
        void ShadeSurface(ClwScene const& scene, int pass, int2 const& tile_size);
        // Evaluate volume
        void EvaluateVolume(ClwScene const& scene, int pass, int2 const& tile_size);
        // Handle missing rays
        void ShadeMiss(ClwScene const& scene, int pass, int2 const& tile_size);
        // Gather light samples and account for visibility
        void GatherLightSamples(ClwScene const& scene, int pass, int2 const& tile_size);
        // Restore pixel indices after compaction
        void RestorePixelIndices(int pass, int2 const& tile_size);
        // Convert intersection info to compaction predicate
        void FilterPathStream(int pass, int2 const& tile_size);
        // Integrate volume
        void ShadeVolume(ClwScene const& scene, int pass, int2 const& tile_size);
        // Shade background
        void ShadeBackground(ClwScene const& scene, int pass, int2 const& tile_size);
        // Fill necessary AOVs
        void FillAOVs(ClwScene const& scene, int2 const& tile_origin, int2 const& tile_size);
        // Generate index domain
        void GenerateTileDomain(int2 const& output_size, int2 const& tile_origin, int2 const& tile_size, int2 const& subtile_size);
        // Find non-zero AOV
        Output* FindFirstNonZeroOutput(bool include_color = true) const;

    public:
        // CL context
        CLWContext m_context;
        // Scene tracker
        ClwSceneController m_scene_controller;

        // GPU data
        struct PathState;
        struct RenderData;

        std::unique_ptr<RenderData> m_render_data;

        // Intersector data
        std::vector<RadeonRays::Shape*> m_shapes;

        // Vidmem usage
        // Working set
        size_t m_vidmemws;

    private:
        std::uint32_t m_num_bounces;
        mutable std::uint32_t m_framecnt;
    };

}
