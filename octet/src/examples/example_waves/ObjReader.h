
namespace octet {
  class ObjReader {
  public: 

    static std::string load_file(const char* file_path) {
      std::ifstream file(file_path);
      if (file.bad() || !file.is_open()) { return nullptr; }
      char buffer[2048];
      std::string out;
      while (!file.eof()) {
        file.getline(buffer, sizeof(buffer));
        out += buffer;
        out += "\n";
      }
      return out;
    }

    static ref<mesh> load_mesh_file(char* file_name) {
      std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
      std::vector< float > temp_vertices;
      std::vector< float > temp_uvs;
      std::vector< float > temp_normals;

      FILE * file = fopen(file_name, "r");
      if (file == NULL) {
        printf("Impossible to open the file !\n");
        return false;
      }
      while (1) {

        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
          break; // EOF = End Of File. Quit the loop.

        if (strcmp(lineHeader, "v") == 0) {
          float x, y, z;
          fscanf(file, "%f %f %f\n", &x, &y, &z);
          temp_vertices.push_back(x);
          temp_vertices.push_back(y);
          temp_vertices.push_back(z);
        }
        else if (strcmp(lineHeader, "vt") == 0) {
          float tx, ty;
          fscanf(file, "%f %f\n", &tx, &ty);
          temp_uvs.push_back(tx);
          temp_uvs.push_back(ty);
        }
        else if (strcmp(lineHeader, "vn") == 0) {
          float nx, ny, nz;
          fscanf(file, "%f %f %f\n", &nx, &ny, &nz);
          temp_normals.push_back(nx);
          temp_normals.push_back(ny);
          temp_normals.push_back(nz);
        }
        else if (strcmp(lineHeader, "f") == 0) {
          std::string vertex1, vertex2, vertex3;
          unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
          int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
          if (matches != 9) {
            matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
            if (matches != 6) {
              matches = fscanf(file, "%d/%d/ %d/%d/ %d/%d/\n", &vertexIndex[0], &uvIndex[0], &vertexIndex[1], &uvIndex[1], &vertexIndex[2], &uvIndex[2]);
              if (matches != 6) {
                printf("File can't be read by our simple parser : ( Try exporting with other options\n");
                return false;
              }
            }
            
          }
          vertexIndices.push_back(vertexIndex[0]);
          vertexIndices.push_back(vertexIndex[1]);
          vertexIndices.push_back(vertexIndex[2]);
          uvIndices.push_back(uvIndex[0]);
          uvIndices.push_back(uvIndex[1]);
          uvIndices.push_back(uvIndex[2]);
          normalIndices.push_back(normalIndex[0]);
          normalIndices.push_back(normalIndex[1]);
          normalIndices.push_back(normalIndex[2]);
        }

      }

      std::vector<float> data;

      


      ref<mesh> load_mesh = new mesh(temp_vertices.size(), vertexIndices.size());

      // allocate vertices and indices into OpenGL buffers
      load_mesh->allocate(sizeof(float) * temp_vertices.size(), sizeof(uint32_t) * vertexIndices.size());
      load_mesh->set_params(sizeof(float), vertexIndices.size(), temp_vertices.size()/3, GL_TRIANGLES, GL_UNSIGNED_INT);
      load_mesh->set_first_index(0);
      load_mesh->set_default_attributes();

      load_mesh->get_indices()->assign(&vertexIndices[0], 0, sizeof(uint32_t) * vertexIndices.size());
      load_mesh->get_vertices()->assign(&temp_vertices[0], 0, sizeof(float) * temp_vertices.size());

      return load_mesh;
    }



    static ref<mesh> load_mesh_fileold(char* file_name) {
      std::vector<float> verticies;
      std::vector<float> normals;
      std::vector<uint32_t> face_vert;
      std::vector<uint32_t> face_norm;

      std::ifstream is(file_name);
      if (is.bad() || !is.is_open()) return NULL;
      // store the line here
      char buffer[2048];
      // loop over lines
      while (!is.eof()) {
        is.getline(buffer, sizeof(buffer));
        // loop over columns
        char *b = buffer;
        //printf("%c\n", *b);
        if (*b == '#' || *b == '\0') continue; // Ignore lines starting with #

        char *line = strtok(b, " ");
        while (line != NULL) {
          char* type = line;
          line = strtok(NULL, " ");
          while (line != NULL) {
            if (!std::strcmp(type, "v")) {
              verticies.push_back(std::stof(line));
            }
            else if (!std::strcmp(type, "vn")) {
              normals.push_back(std::stof(line));
            }
            else if (!std::strcmp(type, "f")) {
              char tokenBuff[128];
              strcpy(tokenBuff, line);
              char *faceToken = strtok(tokenBuff, "//");
              face_vert.push_back(std::stoi(faceToken));
              faceToken = strtok(NULL, "//");
              face_norm.push_back(std::stoi(faceToken));
              faceToken = strtok(NULL, "//");
            }

            line = strtok(NULL, " ");
          }
        }
      }
      printf("Num of verts: %d\n", verticies.size() / 3);
      printf("Num of norms: %d\n", normals.size() / 3);
      printf("Num of faces: %d\n", face_vert.size() / 3);

      ref<mesh> load_mesh = new mesh(verticies.size(), face_vert.size());

      // allocate vertices and indices into OpenGL buffers
      load_mesh->allocate(sizeof(float) * verticies.size(), sizeof(uint32_t) * face_vert.size());
      load_mesh->set_params(sizeof(uint32_t), face_vert.size(), verticies.size(), GL_TRIANGLES, GL_UNSIGNED_INT);

      load_mesh->add_attribute(attribute_pos, 3, GL_FLOAT, 0);

      load_mesh->get_indices()->assign(&face_vert[0], 0, sizeof(unsigned int)*face_vert.size());
      load_mesh->get_vertices()->assign(&verticies[0], 0, sizeof(float)*verticies.size());

      return load_mesh;
    }


    static std::vector<float> load_file_vertdata(char* file_name) {
      std::vector<float> verticies;
      std::vector<float> normals;
      std::vector<uint32_t> face_vert;
      std::vector<uint32_t> face_norm;

      std::ifstream is(file_name);
      //if (is.bad() || !is.is_open()) return NULL;
      // store the line here
      char buffer[2048];
      // loop over lines
      while (!is.eof()) {
        is.getline(buffer, sizeof(buffer));
        // loop over columns
        char *b = buffer;
        //printf("%c\n", *b);
        if (*b == '#') continue; // Ignore lines starting with #

                                 //std::string sett = b;
                                 //settings.push_back(sett);

        char *line = strtok(b, " ");
        while (line != NULL) {
          char* type = line;
          line = strtok(NULL, " ");
          while (line != NULL) {
            if (!std::strcmp(type, "v")) {
              verticies.push_back(std::stof(line));
              printf("vertex: %s\n", line);
            }
            else if (!std::strcmp(type, "vn")) {
              normals.push_back(std::stof(line));
            }
            else if (!std::strcmp(type, "f")) {
              char *face = strtok(line, "//");
              face_vert.push_back(std::stoi(face));
              face = strtok(NULL, "//");
              face_norm.push_back(std::stoi(face));
            }
            line = strtok(NULL, " ");
          }
        }
      }
      printf("Num of verts: %d\n", verticies.size() / 3);
      printf("Num of norms: %d\n", normals.size() / 3);
      printf("Num of faces: %d\n", face_vert.size() / 3);

      ref<mesh> load_mesh = new mesh();

      // allocate vertices and indices into OpenGL buffers
      load_mesh->allocate(sizeof(float) * verticies.size(), sizeof(uint32_t) * face_vert.size());
      load_mesh->set_params(sizeof(uint32_t), face_vert.size(), verticies.size(), GL_TRIANGLES, GL_UNSIGNED_INT);

      // describe the structure of my_vertex to OpenGL
      load_mesh->add_attribute(attribute_pos, 3, GL_FLOAT, 0);
      //load_mesh->add_attribute(attribute_normal, 3, GL_FLOAT, 12);

      gl_resource::wolock il(load_mesh->get_indices());
      uint32_t *indx = il.u32();
      memcpy(indx, face_vert.data(), sizeof(uint32_t)*face_vert.size());

      gl_resource::wolock vl(load_mesh->get_vertices());
      float *vtx = (float *)vl.u8();
      memcpy(vtx, verticies.data(), sizeof(float)*verticies.size());

      return verticies;
    }
    static std::vector<int> load_file_facedata(char* file_name) {
      std::vector<float> verticies;
      std::vector<float> normals;
      std::vector<int> face_vert;
      std::vector<int> face_norm;

      std::ifstream is(file_name);
      //if (is.bad() || !is.is_open()) return NULL;
      // store the line here
      char buffer[2048];
      // loop over lines
      while (!is.eof()) {
        is.getline(buffer, sizeof(buffer));
        // loop over columns
        char *b = buffer;
        //printf("%c\n", *b);
        if (*b == '#') continue; // Ignore lines starting with #

                                 //std::string sett = b;
                                 //settings.push_back(sett);

        char *line = strtok(b, " ");
        while (line != NULL) {
          char* type = line;
          line = strtok(NULL, " ");
          while (line != NULL) {
            if (!std::strcmp(type, "v")) {
              verticies.push_back(std::stof(line));
              printf("vertex: %s\n", line);
            }
            else if (!std::strcmp(type, "vn")) {
              normals.push_back(std::stof(line));
            }
            else if (!std::strcmp(type, "f")) {
              char *face = strtok(line, "//");
              face_vert.push_back(std::stoi(face));
              face = strtok(NULL, "//");
              face_norm.push_back(std::stoi(face));
            }
            line = strtok(NULL, " ");
          }
        }
      }
      printf("Num of verts: %d\n", verticies.size() / 3);
      printf("Num of norms: %d\n", normals.size() / 3);
      printf("Num of faces: %d\n", face_vert.size() / 3);

      ref<mesh> load_mesh = new mesh();

      // allocate vertices and indices into OpenGL buffers
      load_mesh->allocate(sizeof(float) * verticies.size(), sizeof(uint32_t) * face_vert.size());
      load_mesh->set_params(sizeof(uint32_t), face_vert.size(), verticies.size(), GL_TRIANGLES, GL_UNSIGNED_INT);

      // describe the structure of my_vertex to OpenGL
      load_mesh->add_attribute(attribute_pos, 3, GL_FLOAT, 0);
      //load_mesh->add_attribute(attribute_normal, 3, GL_FLOAT, 12);

      gl_resource::wolock il(load_mesh->get_indices());
      uint32_t *indx = il.u32();
      memcpy(indx, face_vert.data(), sizeof(uint32_t)*face_vert.size());

      gl_resource::wolock vl(load_mesh->get_vertices());
      float *vtx = (float *)vl.u8();
      memcpy(vtx, verticies.data(), sizeof(float)*verticies.size());

      return face_vert;
    }
  };
}