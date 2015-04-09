//
//   Copyright 2013 Pixar
//
//   Licensed under the Apache License, Version 2.0 (the "Apache License")
//   with the following modification; you may not use this file except in
//   compliance with the Apache License and the following modification to it:
//   Section 6. Trademarks. is deleted and replaced with:
//
//   6. Trademarks. This License does not grant permission to use the trade
//      names, trademarks, service marks, or product names of the Licensor
//      and its affiliates, except as required to comply with Section 4(c) of
//      the License and to reproduce the content of the NOTICE file.
//
//   You may obtain a copy of the Apache License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the Apache License with the above modification is
//   distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//   KIND, either express or implied. See the Apache License for the specific
//   language governing permissions and limitations under the Apache License.
//

#ifndef FAR_PATCH_TABLES_H
#define FAR_PATCH_TABLES_H

#include "../version.h"

#include "../far/interpolate.h"
#include "../far/patchDescriptor.h"
#include "../far/stencilTables.h"
#include "../far/stencilTables.h"

#include "../sdc/options.h"

#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <vector>
#include <map>

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

namespace Far {

/// \brief Container for arrays of parametric patches
///
/// PatchTables contain topology and parametric information about the patches
/// generated by the Refinement process. Patches in the tables are sorted into
/// arrays based on their PatchDescriptor Type.
///
/// Note : PatchTables can be accessed either using a PatchHandle or a
///        combination of array and patch indices.
///
/// XXXX manuelk we should add a PatchIterator that can dereference into
///              a PatchHandle for fast linear traversal of the tables
///
class PatchTables {

public:

    /// \brief Handle that can be used as unique patch identifier within PatchTables
    class PatchHandle {
    // XXXX manuelk members will eventually be made private
    public:

        friend class PatchTables;
        friend class PatchMap;

        Index arrayIndex, // Array index of the patch
              patchIndex, // Absolute Index of the patch
              vertIndex;  // Relative offset to the first CV of the patch in array
    };

public:

    /// \brief Copy constructor
    PatchTables(PatchTables const & src);

    /// \brief Destructor
    ~PatchTables();

    /// \brief True if the patches are of feature adaptive types
    bool IsFeatureAdaptive() const;

    /// \brief Returns the total number of control vertex indices in the tables
    int GetNumControlVerticesTotal() const {
        return (int)_patchVerts.size();
    }

    /// \brief Returns the total number of patches stored in the tables
    int GetNumPatchesTotal() const;

    /// \brief Returns max vertex valence
    int GetMaxValence() const { return _maxValence; }

    /// \brief Returns the total number of ptex faces in the mesh
    int GetNumPtexFaces() const { return _numPtexFaces; }


    //@{
    ///  @name Individual patches
    ///
    /// \anchor individual_patches
    ///
    /// \brief Accessors for individual patches
    ///

    /// \brief Returns the PatchDescriptor for the patches in array 'array'
    PatchDescriptor GetPatchDescriptor(PatchHandle const & handle) const;

    /// \brief Returns the control vertex indices for the patch identified by 'handle'
    ConstIndexArray GetPatchVertices(PatchHandle const & handle) const;

    /// \brief Returns a PatchParam for the patch identified by 'handle'
    PatchParam GetPatchParam(PatchHandle const & handle) const;

    /// \brief Returns the control vertex indices for the patch 'patch' in array 'array'
    ConstIndexArray GetPatchVertices(int array, int patch) const;

    /// \brief Returns the PatchParam for the patch 'patch' in array 'array'
    PatchParam GetPatchParam(int array, int patch) const;
    //@}


    //@{
    ///  @name Arrays of patches
    ///
    /// \anchor arrays_of_patches
    ///
    /// \brief Accessors for arrays of patches of the same type
    ///

    /// \brief Returns the number of patch arrays in the table
    int GetNumPatchArrays() const;

    /// \brief Returns the number of patches in patch array 'array'
    int GetNumPatches(int array) const;

    /// \brief Returns the number of control vertices in patch array 'array'
    int GetNumControlVertices(int array) const;

    /// \brief Returns the PatchDescriptor for the patches in array 'array'
    PatchDescriptor GetPatchArrayDescriptor(int array) const;

    /// \brief Returns the control vertex indices for the patches in array 'array'
    ConstIndexArray GetPatchArrayVertices(int array) const;

    /// \brief Returns the PatchParams for the patches in array 'array'
    ConstPatchParamArray const GetPatchParams(int array) const;
    //@}


    //@{
    ///  @name End-Cap patches
    ///
    /// \anchor end_cap_patches
    ///
    /// \brief Accessors for end-cap patch additional data
    ///

    typedef Vtr::ConstArray<unsigned int> ConstQuadOffsetsArray;

    /// \brief Returns the 'QuadOffsets' for the Gregory patch identified by 'handle'
    ConstQuadOffsetsArray GetPatchQuadOffsets(PatchHandle const & handle) const;

    typedef std::vector<Index> VertexValenceTable;

    /// \brief Returns the 'VertexValences' table (vertex neighborhoods table)
    VertexValenceTable const & GetVertexValenceTable() const {
        return _vertexValenceTable;
    }

    /// \brief Returns a stencil table for the control vertices of end-cap patches
    StencilTables const * GetEndCapVertexStencilTables() const { return _endcapVertexStencilTables; }
    StencilTables const * GetEndCapVaryingStencilTables() const { return _endcapVaryingStencilTables; }

    //@}


    //@{
    ///  @name Single-crease patches
    ///
    /// \anchor single_crease_patches
    ///
    /// \brief Accessors for single-crease patch edge sharpness
    ///

    /// \brief Returns the crease sharpness for the patch identified by 'handle'
    ///        if it is a single-crease patch, or 0.0f
    float GetSingleCreasePatchSharpnessValue(PatchHandle const & handle) const;

    /// \brief Returns the crease sharpness for the patch 'patch' in array 'array'
    ///        if it is a single-crease patch, or 0.0f
    float GetSingleCreasePatchSharpnessValue(int array, int patch) const;
    //@}


    //@{
    ///  @name Face-varying channels
    ///
    /// \anchor face_varying_channels
    ///
    /// \brief Accessors for face-varying channels
    ///

    /// \brief Returns the number of face-varying channels
    int GetNumFVarChannels() const;


    /// \brief Returns the interpolation mode for a given channel
    Sdc::Options::FVarLinearInterpolation GetFVarChannelLinearInterpolation(int channel) const;


    /// \brief Returns a descriptor for a given patch in a channel
    PatchDescriptor::Type GetFVarPatchType(int channel, PatchHandle const & handle) const;

    /// \brief Returns a descriptor for a given patch in a channel
    PatchDescriptor::Type GetFVarPatchType(int channel, int array, int patch) const;

    /// \brief Returns an array of descriptors for the patches in a channel
    Vtr::ConstArray<PatchDescriptor::Type> GetFVarPatchTypes(int channel) const;


    /// \brief Returns the value indices for a given patch in a channel
    ConstIndexArray GetFVarPatchValues(int channel, PatchHandle const & handle) const;

    /// \brief Returns the value indices for a given patch in a channel
    ConstIndexArray GetFVarPatchValues(int channel, int array, int patch) const;


    /// \brief Returns an array of value indices for the patches in a channel
    ConstIndexArray GetFVarPatchesValues(int channel) const;
    //@}


    //@{
    ///  @name Direct accessors
    ///
    /// \warning These direct accessors are left for convenience, but they are
    ///          likely going to be deprecated in future releases
    ///

    typedef std::vector<Index> PatchVertsTable;

    /// \brief Get the table of patch control vertices
    PatchVertsTable const & GetPatchControlVerticesTable() const { return _patchVerts; }

    /// \brief Returns the PatchParamTable (PatchParams order matches patch array sorting)
    PatchParamTable const & GetPatchParamTable() const { return _paramTable; }

    /// \brief Returns a sharpness index table for each patch (if exists)
    std::vector<Index> const &GetSharpnessIndexTable() const { return _sharpnessIndices; }

    /// \brief Returns sharpness values table
    std::vector<float> const &GetSharpnessValues() const { return _sharpnessValues; }

    typedef std::vector<unsigned int> QuadOffsetsTable;

    /// \brief Returns the quad-offsets table
    QuadOffsetsTable const & GetQuadOffsetsTable() const {
        return _quadOffsetsTable;
    }
    //@}

    /// debug helper
    void print() const;

public:

    //@{
    ///  @name Interpolation methods
    //

    /// \brief Interpolate the (s,t) parametric location of a *bilinear* patch
    ///
    /// \note This method can only be used on uniform PatchTables of quads (see
    ///       IsFeatureAdaptive() method)
    ///
    /// @param handle  A patch handle indentifying the sub-patch containing the
    ///                (s,t) location
    ///
    /// @param s       Patch coordinate (in coarse face normalized space)
    ///
    /// @param t       Patch coordinate (in coarse face normalized space)
    ///
    /// @param src     Source primvar buffer (control vertices data)
    ///
    /// @param dst     Destination primvar buffer (limit surface data)
    ///
    template <class T, class U> void EvaluateBilinear(PatchHandle const & handle,
        float s, float t, T const & src, U & dst) const;


    /// \brief Interpolate the (s,t) parametric location of a *bicubic* patch
    ///
    /// \note This method can only be used on feature adaptive PatchTables (ie.
    ///       IsFeatureAdaptive() is false)
    ///
    /// @param handle  A patch handle indentifying the sub-patch containing the
    ///                (s,t) location
    ///
    /// @param s       Patch coordinate (in coarse face normalized space)
    ///
    /// @param t       Patch coordinate (in coarse face normalized space)
    ///
    /// @param src     Source primvar buffer (control vertices data)
    ///
    /// @param dst     Destination primvar buffer (limit surface data)
    ///
    template <class T, class U> void Evaluate(PatchHandle const & handle,
        float s, float t, T const & src, U & dst) const;

    /// \brief Interpolate the (s,t) parametric location of a *bicubic*
    ///        face-varying patch
    ///
    /// @param channel The face-varying primvar channel
    ///
    /// @param handle  A patch handle indentifying the sub-patch containing the
    ///                (s,t) location
    ///
    /// @param s       Patch coordinate (in coarse face normalized space)
    ///
    /// @param t       Patch coordinate (in coarse face normalized space)
    ///
    /// @param src     Source primvar buffer (control vertices data)
    ///
    /// @param dst     Destination primvar buffer (limit surface data)
    ///
    template <class T, class U> void EvaluateFaceVarying(int channel,
        PatchHandle const & handle, float s, float t, T const & src, U & dst) const;

    //@}

protected:

    friend class PatchTablesFactory;

    // Factory constructor
    PatchTables(int maxvalence);

    Index getPatchIndex(int array, int patch) const;

    PatchParamArray getPatchParams(int arrayIndex);

    Index * getSharpnessIndices(Index arrayIndex);
    float * getSharpnessValues(Index arrayIndex);

private:

    //
    // Patch arrays
    //

    struct PatchArray;
    typedef std::vector<PatchArray> PatchArrayVector;

    PatchArray & getPatchArray(Index arrayIndex);
    PatchArray const & getPatchArray(Index arrayIndex) const;

    void reservePatchArrays(int numPatchArrays);
    void pushPatchArray(PatchDescriptor desc, int npatches,
        Index * vidx, Index * pidx, Index * qoidx=0);

    IndexArray getPatchArrayVertices(int arrayIndex);

    Index findPatchArray(PatchDescriptor desc);


    //
    // FVar patch channels
    //

    struct FVarPatchChannel;
    typedef std::vector<FVarPatchChannel> FVarPatchChannelVector;

    FVarPatchChannel & getFVarPatchChannel(int channel);
    FVarPatchChannel const & getFVarPatchChannel(int channel) const;

    void allocateFVarPatchChannels(int numChannels);
    void allocateChannelValues(int channel, int numPatches, int numVerticesTotal);

    void setFVarPatchChannelLinearInterpolation(int channel,
        Sdc::Options::FVarLinearInterpolation interpolation);

    void setFVarPatchChannelPatchesType(int channel, PatchDescriptor::Type type);

    PatchDescriptor::Type getFVarPatchType(int channel, int patch) const;
    Vtr::Array<PatchDescriptor::Type> getFVarPatchTypes(int channel);

    IndexArray getFVarPatchesValues(int channel);
    ConstIndexArray getFVarPatchValues(int channel, int patch) const;

    void setBicubicFVarPatchChannelValues(int channel, int patchSize, std::vector<Index> const & values);

private:

    //
    // Topology
    //

    int _maxValence,   // highest vertex valence found in the mesh
        _numPtexFaces; // total number of ptex faces

    PatchArrayVector     _patchArrays;  // Vector of descriptors for arrays of patches

    std::vector<Index>   _patchVerts;   // Indices of the control vertices of the patches

    PatchParamTable      _paramTable;   // PatchParam bitfields (one per patch)

    //
    // Extraordinary vertex closed-form evaluation
    //

    // XXXX manuelk end-cap stencils will obsolete the other tables

    StencilTables const * _endcapVertexStencilTables;
    StencilTables const * _endcapVaryingStencilTables;
    QuadOffsetsTable     _quadOffsetsTable;   // Quad offsets (for Gregory patches)
    VertexValenceTable   _vertexValenceTable; // Vertex valence table (for Gregory patches)

    //
    // Face-varying data
    //

    FVarPatchChannelVector _fvarChannels;

    //
    // 'single-crease' patch sharpness tables
    //

    std::vector<Index>   _sharpnessIndices; // Indices of single-crease sharpness (one per patch)
    std::vector<float>   _sharpnessValues;  // Sharpness values.
};

// XXXX manuelk evaluation should have the following interface :
//    - EvaluateVertex<>()
//    - EvaluateVarying<>()
//    - EvaluateFaceVarying<>()
// this refactor is pending the move of fvar channels as a private data
// structure inside PatchTables, along with the addition of accessors that
// use PatchHandle and work that hides the indexing of the patches inside
// the tables

// Interpolates primvar limit at the given parametric location on a patch
template <class T, class U>
inline void
PatchTables::Evaluate(PatchHandle const & handle, float s, float t,
    T const & src, U & dst) const {

    assert(IsFeatureAdaptive());

    PatchParam::BitField const & bits = _paramTable[handle.patchIndex].bitField;

    PatchDescriptor::Type ptype =
        GetPatchArrayDescriptor(handle.arrayIndex).GetType();

    dst.Clear();

    float Q[16], Qd1[16], Qd2[16];

    if (ptype>=PatchDescriptor::REGULAR and ptype<=PatchDescriptor::CORNER) {

        GetBSplineWeights(bits, s, t, Q, Qd1, Qd2);

        ConstIndexArray cvs = GetPatchVertices(handle);

        switch (ptype) {
            case PatchDescriptor::REGULAR:
                InterpolateRegularPatch(cvs.begin(), Q, Qd1, Qd2, src, dst);
                break;
            case PatchDescriptor::SINGLE_CREASE:
                // TODO: implement InterpolateSingleCreasePatch().
                //InterpolateRegularPatch(cvs, Q, Qd1, Qd2, src, dst);
                break;
            case PatchDescriptor::BOUNDARY:
                InterpolateBoundaryPatch(cvs.begin(), Q, Qd1, Qd2, src, dst);
                break;
            case PatchDescriptor::CORNER:
                InterpolateCornerPatch(cvs.begin(), Q, Qd1, Qd2, src, dst);
                break;
            case PatchDescriptor::GREGORY:
            case PatchDescriptor::GREGORY_BOUNDARY:
                assert(0);
                break;
            default:
                assert(0);
        }
    } else if (ptype==PatchDescriptor::GREGORY_BASIS) {

        assert(_endcapVertexStencilTables);

        GetBezierWeights(bits, s, t, Q, Qd1, Qd2);
        InterpolateGregoryPatch(_endcapVertexStencilTables, handle.vertIndex,
            s, t, Q, Qd1, Qd2, src, dst);

    } else if (ptype==PatchDescriptor::QUADS) {

        ConstIndexArray cvs = GetPatchVertices(handle);

        GetBilinearWeights(bits, s, t, Q, Qd1, Qd2);
        InterpolateBilinearPatch(cvs.begin(), Q, Qd1, Qd2, src, dst);

    } else {
        assert(0);
    }
}

// Interpolates the limit position of a parametric location on a face-varying
// patch
// XXXX manuelk this method is very similar to the vertex Evaluate<>() method
//              -> we should eventually merge them
template <class T, class U>
inline void
PatchTables::EvaluateFaceVarying(int channel, PatchHandle const & handle,
    float s, float t, T const & src, U & dst) const {

    ConstIndexArray cvs = GetFVarPatchValues(channel, handle);

    PatchDescriptor::Type type = GetFVarPatchType(channel, handle);

    PatchParam::BitField bits;
    bits.Clear();

    float Q[16], Qd1[16], Qd2[16];

    switch (type) {
        case PatchDescriptor::QUADS:
            GetBilinearWeights(bits, s, t, Q, Qd1, Qd2);
            InterpolateBilinearPatch(cvs.begin(), Q, Qd1, Qd2, src, dst);
            break;
        case PatchDescriptor::TRIANGLES:
            assert("not implemented yet");
        case PatchDescriptor::REGULAR:
            GetBSplineWeights(bits, s, t, Q, Qd1, Qd2);
            InterpolateRegularPatch(cvs.begin(), Q, Qd1, Qd2, src, dst);
            break;
        case PatchDescriptor::BOUNDARY:
            GetBSplineWeights(bits, s, t, Q, Qd1, Qd2);
            InterpolateBoundaryPatch(cvs.begin(), Q, Qd1, Qd2, src, dst);
            break;
        case PatchDescriptor::CORNER:
            GetBSplineWeights(bits, s, t, Q, Qd1, Qd2);
            InterpolateCornerPatch(cvs.begin(), Q, Qd1, Qd2, src, dst);
            break;
        default:
            assert(0);

        // XXXX manuelk - how do we handle end-patches ?
        //              - is there a bicubic patch that we could use to reduce
        //                isolation of bilinear boundaries with smooth a interior ?
    }
}

// Interpolates primvar at the given parametric location on a bilinear patch
template <class T, class U>
inline void
PatchTables::EvaluateBilinear(PatchHandle const & handle, float s, float t,
    T const & src, U & dst) const {

    ConstIndexArray cvs = GetPatchVertices(handle);
    assert(cvs.size()==4);

    PatchParam::BitField const & bits =
        _paramTable[handle.patchIndex].bitField;

    dst.Clear();

    float Q[4], Qd1[4], Qd2[4];
    GetBilinearWeights(bits, s, t, Q, Qd1, Qd2);

    InterpolateBilinearPatch(cvs.begin(), Q, Qd1, Qd2, src, dst);
}


} // end namespace Far

} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;

} // end namespace OpenSubdiv

#endif /* FAR_PATCH_TABLES */
