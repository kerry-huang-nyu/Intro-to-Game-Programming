// Utility.cpp
#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION
#define NUMBER_OF_TEXTURES 1
#define LEVEL_OF_DETAIL    0
#define TEXTURE_BORDER     0
#define FONTBANK_SIZE      16

#include "Utility.h"
#include <SDL_image.h>
#include "stb_image.h"


GLuint Utility::load_texture(const char* filepath) {
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint texture_id;
    glGenTextures(NUMBER_OF_TEXTURES, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return texture_id;
}

void Utility::draw_text(ShaderProgram* program, GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position)
{
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;


    std::vector<float> vertices;
    std::vector<float> texture_coordinates;

    for (int i = 0; i < text.size(); i++) {
        // 1. Get their index in the spritesheet, as well as their offset (i.e. their position
        //    relative to the whole sentence)
        int spritesheet_index = (int)text[i];  // ascii value of character
        float offset = (screen_size + spacing) * i;

        // 2. Using the spritesheet index, we can calculate our U- and V-coordinates
        float u_coordinate = (float)(spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
        float v_coordinate = (float)(spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

        // 3. Inset the current pair in both vectors
        vertices.insert(vertices.end(), {
            offset + (-0.5f * screen_size), 0.5f * screen_size,
            offset + (-0.5f * screen_size), -0.5f * screen_size,
            offset + (0.5f * screen_size), 0.5f * screen_size,
            offset + (0.5f * screen_size), -0.5f * screen_size,
            offset + (0.5f * screen_size), 0.5f * screen_size,
            offset + (-0.5f * screen_size), -0.5f * screen_size,
            });

        texture_coordinates.insert(texture_coordinates.end(), {
            u_coordinate, v_coordinate,
            u_coordinate, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate + width, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate, v_coordinate + height,
            });
    }

    // 4. And render all of them using the pairs
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);

    program->set_model_matrix(model_matrix);
    glUseProgram(program->get_program_id());

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates.data());
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}


//adapted from: https://stackoverflow.com/questions/22444450/drawing-circle-with-opengl But these functions and Glvertex2f has been deprecated 
//Instead use this https://faun.pub/draw-circle-in-opengl-c-2da8d9c2c103 

std::vector<float> Utility::buildCircle(float radius, int vCount)
{

    float angle = 360.0f / vCount;      //size of each angle 
    //triangle count is number of vertices - 2 due to the overlap 

    std::vector<float> vertices; //center coordinates are like this 

    // positions
    for (int i = 0; i < vCount; i++)
    {
        float currentAngle = angle * i;


        float x = radius * cos(glm::radians(currentAngle));
        float y = radius * sin(glm::radians(currentAngle));
        //float z = 0.0f;

        vertices.push_back(x);
        vertices.push_back(y);
    }

    std::vector<float> triangles;

    for (int i = 0; i < vCount; i++) {
        //center point 
        triangles.push_back(0.0);
        triangles.push_back(0.0);

        //first point 
        triangles.push_back(vertices[i * 2 % vertices.size()]);
        triangles.push_back(vertices[((i * 2) + 1) % vertices.size()]);

        //2nd point 
        triangles.push_back(vertices[(i + 1) * 2 % vertices.size()]);
        triangles.push_back(vertices[((i + 1) * 2  + 1) % vertices.size()]);

    }

    return triangles;

}



void Utility::render(ShaderProgram* program, glm::mat4 model_matrix, float radius, int vCount, std::ofstream& log)
{ 
    
    //glClear(GL_COLOR_BUFFER_BIT); //idk if you need to have this tbh 

    // Step 2

    program->set_model_matrix(model_matrix);

   

    std::vector<float> triangles = Utility::buildCircle(radius, vCount);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, triangles.data()); //instead of triangles.data just draw 1 triangle 
    glEnableVertexAttribArray(program->get_position_attribute());


    glDrawArrays(GL_TRIANGLES, 0, vCount * 3); //instead of vCount * 3 just do 3 

    glDisableVertexAttribArray(program->get_position_attribute());

}