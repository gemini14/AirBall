xof 0303txt 0032
template XSkinMeshHeader {
 <3cf169ce-ff7c-44ab-93c0-f78f62d172e2>
 WORD nMaxSkinWeightsPerVertex;
 WORD nMaxSkinWeightsPerFace;
 WORD nBones;
}

template VertexDuplicationIndices {
<b8d65549-d7c9-4995-89cf-53a9a8b031e3>
 DWORD nIndices;
 DWORD nOriginalVertices;
 array DWORD indices[nIndices];
}

template SkinWeights {
 <6f0d123b-bad2-4167-a0d0-80224f25fabb>
 STRING transformNodeName;
 DWORD nWeights;
 array DWORD vertexIndices[nWeights];
 array FLOAT weights[nWeights];
 Matrix4x4 matrixOffset;
}


Frame Scene_Root {


 FrameTransformMatrix {
  1.000000, 0.000000, 0.000000, 0.000000,
  0.000000, 1.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 1.000000, 0.000000,
  0.000000, 0.000000, 0.000000, 1.000000;;
 }

  Frame Sphere02 {

   FrameTransformMatrix {
    1.000000, 0.000000, 0.000000, 0.000000,
    0.000000, 1.000000, 0.000000, 0.000000,
    0.000000, 0.000000, 1.000000, 0.000000,
    0.000000, 0.000000, 0.000000, 1.000000;;
   }

   Mesh {
    79;
    0.000000;-0.125000;-0.000000;,
    0.047835;-0.115485;-0.000000;,
    0.033825;-0.115485;-0.033825;,
    0.000000;0.125000;-0.000000;,
    0.033825;0.115485;-0.033825;,
    0.047835;0.115485;-0.000000;,
    0.000000;-0.125000;-0.000000;,
    0.000000;-0.115485;-0.047835;,
    0.000000;0.125000;-0.000000;,
    0.000000;0.115485;-0.047835;,
    0.000000;-0.125000;-0.000000;,
    -0.033825;-0.115485;-0.033825;,
    0.000000;0.125000;-0.000000;,
    -0.033825;0.115485;-0.033825;,
    0.000000;-0.125000;-0.000000;,
    -0.047835;-0.115485;-0.000000;,
    0.000000;0.125000;-0.000000;,
    -0.047835;0.115485;-0.000000;,
    0.000000;-0.125000;-0.000000;,
    -0.033825;-0.115485;0.033825;,
    0.000000;0.125000;-0.000000;,
    -0.033825;0.115485;0.033825;,
    0.000000;-0.125000;-0.000000;,
    -0.000000;-0.115485;0.047835;,
    0.000000;0.125000;-0.000000;,
    -0.000000;0.115485;0.047835;,
    0.000000;-0.125000;-0.000000;,
    0.033825;-0.115485;0.033825;,
    0.000000;0.125000;-0.000000;,
    0.033825;0.115485;0.033825;,
    0.000000;-0.125000;-0.000000;,
    0.047835;-0.115485;-0.000000;,
    0.000000;0.125000;-0.000000;,
    0.047835;0.115485;-0.000000;,
    0.088388;-0.088388;-0.000000;,
    0.062500;-0.088388;-0.062500;,
    0.000000;-0.088388;-0.088388;,
    -0.062500;-0.088388;-0.062500;,
    -0.088388;-0.088388;-0.000000;,
    -0.062500;-0.088388;0.062500;,
    -0.000000;-0.088388;0.088388;,
    0.062500;-0.088388;0.062500;,
    0.088388;-0.088388;-0.000000;,
    0.115485;-0.047835;-0.000000;,
    0.081660;-0.047835;-0.081660;,
    0.000000;-0.047835;-0.115485;,
    -0.081660;-0.047835;-0.081660;,
    -0.115485;-0.047835;-0.000000;,
    -0.081660;-0.047835;0.081660;,
    -0.000000;-0.047835;0.115485;,
    0.081660;-0.047835;0.081660;,
    0.115485;-0.047835;-0.000000;,
    0.125000;0.000000;-0.000000;,
    0.088388;0.000000;-0.088388;,
    0.000000;0.000000;-0.125000;,
    -0.088388;0.000000;-0.088388;,
    -0.125000;0.000000;-0.000000;,
    -0.088388;0.000000;0.088388;,
    -0.000000;0.000000;0.125000;,
    0.088388;0.000000;0.088388;,
    0.125000;0.000000;-0.000000;,
    0.115485;0.047835;-0.000000;,
    0.081660;0.047835;-0.081660;,
    0.000000;0.047835;-0.115485;,
    -0.081660;0.047835;-0.081660;,
    -0.115485;0.047835;-0.000000;,
    -0.081660;0.047835;0.081660;,
    -0.000000;0.047835;0.115485;,
    0.081660;0.047835;0.081660;,
    0.115485;0.047835;-0.000000;,
    0.088388;0.088388;-0.000000;,
    0.062500;0.088388;-0.062500;,
    0.000000;0.088388;-0.088388;,
    -0.062500;0.088388;-0.062500;,
    -0.088388;0.088388;-0.000000;,
    -0.062500;0.088388;0.062500;,
    -0.000000;0.088388;0.088388;,
    0.062500;0.088388;0.062500;,
    0.088388;0.088388;-0.000000;;
    112;
    3;0,2,1;,
    3;3,5,4;,
    3;6,7,2;,
    3;8,4,9;,
    3;10,11,7;,
    3;12,9,13;,
    3;14,15,11;,
    3;16,13,17;,
    3;18,19,15;,
    3;20,17,21;,
    3;22,23,19;,
    3;24,21,25;,
    3;26,27,23;,
    3;28,25,29;,
    3;30,31,27;,
    3;32,29,33;,
    3;1,35,34;,
    3;1,2,35;,
    3;2,7,35;,
    3;35,7,36;,
    3;7,37,36;,
    3;7,11,37;,
    3;11,15,37;,
    3;37,15,38;,
    3;15,39,38;,
    3;15,19,39;,
    3;19,23,39;,
    3;39,23,40;,
    3;23,41,40;,
    3;23,27,41;,
    3;27,31,41;,
    3;41,31,42;,
    3;34,35,43;,
    3;43,35,44;,
    3;35,45,44;,
    3;35,36,45;,
    3;36,37,45;,
    3;45,37,46;,
    3;37,47,46;,
    3;37,38,47;,
    3;38,39,47;,
    3;47,39,48;,
    3;39,49,48;,
    3;39,40,49;,
    3;40,41,49;,
    3;49,41,50;,
    3;41,51,50;,
    3;41,42,51;,
    3;43,53,52;,
    3;43,44,53;,
    3;44,45,53;,
    3;53,45,54;,
    3;45,55,54;,
    3;45,46,55;,
    3;46,47,55;,
    3;55,47,56;,
    3;47,57,56;,
    3;47,48,57;,
    3;48,49,57;,
    3;57,49,58;,
    3;49,59,58;,
    3;49,50,59;,
    3;50,51,59;,
    3;59,51,60;,
    3;52,53,61;,
    3;61,53,62;,
    3;53,63,62;,
    3;53,54,63;,
    3;54,55,63;,
    3;63,55,64;,
    3;55,65,64;,
    3;55,56,65;,
    3;56,57,65;,
    3;65,57,66;,
    3;57,67,66;,
    3;57,58,67;,
    3;58,59,67;,
    3;67,59,68;,
    3;59,69,68;,
    3;59,60,69;,
    3;61,71,70;,
    3;61,62,71;,
    3;62,63,71;,
    3;71,63,72;,
    3;63,73,72;,
    3;63,64,73;,
    3;64,65,73;,
    3;73,65,74;,
    3;65,75,74;,
    3;65,66,75;,
    3;66,67,75;,
    3;75,67,76;,
    3;67,77,76;,
    3;67,68,77;,
    3;68,69,77;,
    3;77,69,78;,
    3;70,71,5;,
    3;5,71,4;,
    3;71,9,4;,
    3;71,72,9;,
    3;72,73,9;,
    3;9,73,13;,
    3;73,17,13;,
    3;73,74,17;,
    3;74,75,17;,
    3;17,75,21;,
    3;75,25,21;,
    3;75,76,25;,
    3;76,77,25;,
    3;25,77,29;,
    3;77,33,29;,
    3;77,78,33;;

    MeshNormals {
     79;
     0.000000;-1.000000;-0.000000;,
     0.382683;-0.923880;-0.000000;,
     0.270598;-0.923880;-0.270598;,
     0.000000;1.000000;-0.000000;,
     0.270598;0.923880;-0.270598;,
     0.382683;0.923880;-0.000000;,
     0.000000;-1.000000;-0.000000;,
     0.000000;-0.923880;-0.382683;,
     0.000000;1.000000;-0.000000;,
     0.000000;0.923880;-0.382683;,
     0.000000;-1.000000;-0.000000;,
     -0.270598;-0.923880;-0.270598;,
     0.000000;1.000000;-0.000000;,
     -0.270598;0.923880;-0.270598;,
     0.000000;-1.000000;-0.000000;,
     -0.382683;-0.923880;-0.000000;,
     0.000000;1.000000;-0.000000;,
     -0.382683;0.923880;-0.000000;,
     0.000000;-1.000000;-0.000000;,
     -0.270598;-0.923880;0.270598;,
     0.000000;1.000000;-0.000000;,
     -0.270598;0.923880;0.270598;,
     0.000000;-1.000000;-0.000000;,
     -0.000000;-0.923880;0.382683;,
     0.000000;1.000000;-0.000000;,
     -0.000000;0.923880;0.382683;,
     0.000000;-1.000000;-0.000000;,
     0.270598;-0.923880;0.270598;,
     0.000000;1.000000;-0.000000;,
     0.270598;0.923880;0.270598;,
     0.000000;-1.000000;-0.000000;,
     0.382683;-0.923880;-0.000000;,
     0.000000;1.000000;-0.000000;,
     0.382683;0.923880;-0.000000;,
     0.707107;-0.707107;-0.000000;,
     0.500000;-0.707107;-0.500000;,
     0.000000;-0.707107;-0.707107;,
     -0.500000;-0.707107;-0.500000;,
     -0.707107;-0.707107;-0.000000;,
     -0.500000;-0.707107;0.500000;,
     -0.000000;-0.707107;0.707107;,
     0.500000;-0.707107;0.500000;,
     0.707107;-0.707107;-0.000000;,
     0.923880;-0.382683;-0.000000;,
     0.653281;-0.382683;-0.653281;,
     0.000000;-0.382683;-0.923880;,
     -0.653281;-0.382683;-0.653281;,
     -0.923880;-0.382683;-0.000000;,
     -0.653281;-0.382683;0.653281;,
     -0.000000;-0.382683;0.923880;,
     0.653281;-0.382683;0.653281;,
     0.923880;-0.382683;-0.000000;,
     1.000000;0.000000;-0.000000;,
     0.707107;0.000000;-0.707107;,
     0.000000;0.000000;-1.000000;,
     -0.707107;0.000000;-0.707107;,
     -1.000000;0.000000;-0.000000;,
     -0.707107;0.000000;0.707107;,
     -0.000000;0.000000;1.000000;,
     0.707107;0.000000;0.707107;,
     1.000000;0.000000;-0.000000;,
     0.923880;0.382683;-0.000000;,
     0.653282;0.382683;-0.653282;,
     0.000000;0.382683;-0.923880;,
     -0.653282;0.382683;-0.653282;,
     -0.923880;0.382683;-0.000000;,
     -0.653282;0.382683;0.653282;,
     -0.000000;0.382683;0.923880;,
     0.653282;0.382683;0.653282;,
     0.923880;0.382683;-0.000000;,
     0.707107;0.707107;-0.000000;,
     0.500000;0.707107;-0.500000;,
     0.000000;0.707107;-0.707107;,
     -0.500000;0.707107;-0.500000;,
     -0.707107;0.707107;-0.000000;,
     -0.500000;0.707107;0.500000;,
     -0.000000;0.707107;0.707107;,
     0.500000;0.707107;0.500000;,
     0.707107;0.707107;-0.000000;;
     112;
     3;0,2,1;,
     3;3,5,4;,
     3;6,7,2;,
     3;8,4,9;,
     3;10,11,7;,
     3;12,9,13;,
     3;14,15,11;,
     3;16,13,17;,
     3;18,19,15;,
     3;20,17,21;,
     3;22,23,19;,
     3;24,21,25;,
     3;26,27,23;,
     3;28,25,29;,
     3;30,31,27;,
     3;32,29,33;,
     3;1,35,34;,
     3;1,2,35;,
     3;2,7,35;,
     3;35,7,36;,
     3;7,37,36;,
     3;7,11,37;,
     3;11,15,37;,
     3;37,15,38;,
     3;15,39,38;,
     3;15,19,39;,
     3;19,23,39;,
     3;39,23,40;,
     3;23,41,40;,
     3;23,27,41;,
     3;27,31,41;,
     3;41,31,42;,
     3;34,35,43;,
     3;43,35,44;,
     3;35,45,44;,
     3;35,36,45;,
     3;36,37,45;,
     3;45,37,46;,
     3;37,47,46;,
     3;37,38,47;,
     3;38,39,47;,
     3;47,39,48;,
     3;39,49,48;,
     3;39,40,49;,
     3;40,41,49;,
     3;49,41,50;,
     3;41,51,50;,
     3;41,42,51;,
     3;43,53,52;,
     3;43,44,53;,
     3;44,45,53;,
     3;53,45,54;,
     3;45,55,54;,
     3;45,46,55;,
     3;46,47,55;,
     3;55,47,56;,
     3;47,57,56;,
     3;47,48,57;,
     3;48,49,57;,
     3;57,49,58;,
     3;49,59,58;,
     3;49,50,59;,
     3;50,51,59;,
     3;59,51,60;,
     3;52,53,61;,
     3;61,53,62;,
     3;53,63,62;,
     3;53,54,63;,
     3;54,55,63;,
     3;63,55,64;,
     3;55,65,64;,
     3;55,56,65;,
     3;56,57,65;,
     3;65,57,66;,
     3;57,67,66;,
     3;57,58,67;,
     3;58,59,67;,
     3;67,59,68;,
     3;59,69,68;,
     3;59,60,69;,
     3;61,71,70;,
     3;61,62,71;,
     3;62,63,71;,
     3;71,63,72;,
     3;63,73,72;,
     3;63,64,73;,
     3;64,65,73;,
     3;73,65,74;,
     3;65,75,74;,
     3;65,66,75;,
     3;66,67,75;,
     3;75,67,76;,
     3;67,77,76;,
     3;67,68,77;,
     3;68,69,77;,
     3;77,69,78;,
     3;70,71,5;,
     3;5,71,4;,
     3;71,9,4;,
     3;71,72,9;,
     3;72,73,9;,
     3;9,73,13;,
     3;73,17,13;,
     3;73,74,17;,
     3;74,75,17;,
     3;17,75,21;,
     3;75,25,21;,
     3;75,76,25;,
     3;76,77,25;,
     3;25,77,29;,
     3;77,33,29;,
     3;77,78,33;;
    }

    MeshTextureCoords {
     79;
     0.000000;1.000000;,
     0.000000;0.875000;,
     0.125000;0.875000;,
     0.000000;0.000000;,
     0.125000;0.125000;,
     0.000000;0.125000;,
     0.125000;1.000000;,
     0.250000;0.875000;,
     0.125000;0.000000;,
     0.250000;0.125000;,
     0.250000;1.000000;,
     0.375000;0.875000;,
     0.250000;0.000000;,
     0.375000;0.125000;,
     0.375000;1.000000;,
     0.500000;0.875000;,
     0.375000;0.000000;,
     0.500000;0.125000;,
     0.500000;1.000000;,
     0.625000;0.875000;,
     0.500000;0.000000;,
     0.625000;0.125000;,
     0.625000;1.000000;,
     0.750000;0.875000;,
     0.625000;0.000000;,
     0.750000;0.125000;,
     0.750000;1.000000;,
     0.875000;0.875000;,
     0.750000;0.000000;,
     0.875000;0.125000;,
     0.875000;1.000000;,
     1.000000;0.875000;,
     0.875000;0.000000;,
     1.000000;0.125000;,
     0.000000;0.750000;,
     0.125000;0.750000;,
     0.250000;0.750000;,
     0.375000;0.750000;,
     0.500000;0.750000;,
     0.625000;0.750000;,
     0.750000;0.750000;,
     0.875000;0.750000;,
     1.000000;0.750000;,
     0.000000;0.625000;,
     0.125000;0.625000;,
     0.250000;0.625000;,
     0.375000;0.625000;,
     0.500000;0.625000;,
     0.625000;0.625000;,
     0.750000;0.625000;,
     0.875000;0.625000;,
     1.000000;0.625000;,
     0.000000;0.500000;,
     0.125000;0.500000;,
     0.250000;0.500000;,
     0.375000;0.500000;,
     0.500000;0.500000;,
     0.625000;0.500000;,
     0.750000;0.500000;,
     0.875000;0.500000;,
     1.000000;0.500000;,
     0.000000;0.375000;,
     0.125000;0.375000;,
     0.250000;0.375000;,
     0.375000;0.375000;,
     0.500000;0.375000;,
     0.625000;0.375000;,
     0.750000;0.375000;,
     0.875000;0.375000;,
     1.000000;0.375000;,
     0.000000;0.250000;,
     0.125000;0.250000;,
     0.250000;0.250000;,
     0.375000;0.250000;,
     0.500000;0.250000;,
     0.625000;0.250000;,
     0.750000;0.250000;,
     0.875000;0.250000;,
     1.000000;0.250000;;
    }

    VertexDuplicationIndices {
     79;
     79;
     0,
     1,
     2,
     3,
     4,
     5,
     6,
     7,
     8,
     9,
     10,
     11,
     12,
     13,
     14,
     15,
     16,
     17,
     18,
     19,
     20,
     21,
     22,
     23,
     24,
     25,
     26,
     27,
     28,
     29,
     30,
     31,
     32,
     33,
     34,
     35,
     36,
     37,
     38,
     39,
     40,
     41,
     42,
     43,
     44,
     45,
     46,
     47,
     48,
     49,
     50,
     51,
     52,
     53,
     54,
     55,
     56,
     57,
     58,
     59,
     60,
     61,
     62,
     63,
     64,
     65,
     66,
     67,
     68,
     69,
     70,
     71,
     72,
     73,
     74,
     75,
     76,
     77,
     78;
    }

    MeshMaterialList {
     1;
     112;
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0;

     Material DefaultMat {
      0.800000;0.800000;0.800000;0.000000;;
      0.000000;
      0.000000;0.000000;0.000000;;
      0.000000;0.000000;0.000000;;
     }
    }

    XSkinMeshHeader {
     1;
     3;
     0;
    }

   }
 }
 }
}

AnimationSet AnimationSet0
{
 Animation
 {
  AnimationKey
  {
   4;
   2;
   0; 16; 1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000;;,
   -1; 16; 1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000;;;
  }
  { Scene_Root }
 }
}
