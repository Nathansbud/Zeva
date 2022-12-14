#include "SceneMaker.h"
#include <iostream>

const std::vector<glm::vec4> props = {
    glm::vec4(1, 0, 0, 1),
    glm::vec4(0, 1, 0, 1),
    glm::vec4(0, 0, 1, 1),
    glm::vec4(0.5, 0.5, 0.5, 1),
    glm::vec4(0.5, 0.5, 0, 1),

    glm::vec4(0.302, 0.933, 0.918, 1),
    glm::vec4(0.455, 0.933, 0.083, 1),
    glm::vec4(1, 0.906, 0, 1),
    glm::vec4(0.941, 0, 1, 1)
};

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> f(0, 1);
std::uniform_int_distribution<> colorIdx(0, props.size() - 1);
std::uniform_int_distribution<> mushIdx(0, 3);

MushroomData* SceneMaker::generateMushroom(int variant, float xOffset, float zOffset) {
    SceneFileMap mushroomTexture;
    mushroomTexture.filename = "../desktop/resources/textures/mushroom" + std::to_string(variant) + ".jpg";
    mushroomTexture.repeatU = true;
    mushroomTexture.repeatV = true;
    mushroomTexture.isUsed = true;

    SceneMaterial shroomterial = SceneMaterial{
        .cAmbient = props[colorIdx(gen)],
        .cDiffuse = props[colorIdx(gen)],
        .cSpecular = props[colorIdx(gen)],
        .shininess = (float) f(gen),
        .textureMap = mushroomTexture
    };

    SceneMaterial stemterial = SceneMaterial{
        .cAmbient = props[colorIdx(gen)],
        .cDiffuse = props[colorIdx(gen)],
        .cSpecular = props[colorIdx(gen)],
        .shininess = (float) f(gen)
    };

    MushroomData* mushroom = nullptr;

    // make primitives
    ScenePrimitive mushtop_prim = ScenePrimitive{.type=PrimitiveType::PRIMITIVE_MUSHTOP, .material = shroomterial};
    ScenePrimitive mushstem_prim = ScenePrimitive{.type=PrimitiveType::PRIMITIVE_CYLINDER, .material = stemterial};

    // make shrinking matrix
    glm::mat4 scaleall = glm::mat4({0.2, 0, 0, 0}, {0, 0.2, 0, 0}, {0, 0, 0.2, 0}, {0, 0, 0, 1});

    float h = heights[variant];
    float cw = capwidths[variant];
    float ch = capheights[variant];

    // init cap ctms
    glm::mat4 ctm = glm::mat4({cw, 0, 0, 0}, {0, ch, 0, 0}, {0, 0, cw, 0}, {xOffset, h, zOffset, 1});
    if(variant == 2) ctm = glm::mat4({cw, 0, 0, 0}, {0, ch, 0, 0}, {0, 0, cw, 0}, {xOffset, h - 2, zOffset, 1});
    glm::mat4 ictm = glm::inverse(ctm);
    glm::mat4 nictm = glm::inverse(glm::transpose(glm::mat3(ctm)));

    RenderShapeData mushtop = RenderShapeData{
        .primitive = mushtop_prim,
        .ctm = scaleall * ctm,
        .ictm = ictm,
        .nictm = nictm
    };

    // init stem ctms
    glm::mat4 sctm = glm::mat4({1/3.0, 0, 0, 0}, {0, h, 0, 0}, {0, 0, 1/3.0, 0}, {xOffset, h/2, zOffset, 1});
    if(variant == 2) sctm = glm::mat4({1/3.0, 0, 0, 0}, {0, h, 0, 0}, {0, 0, 1/3.0, 0}, {xOffset, h/2 - 2, zOffset, 1});
    glm::mat4 sictm = glm::inverse(sctm);
    glm::mat4 snictm = glm::inverse(glm::transpose(glm::mat3(sctm)));

    glm::mat4 ctmShape = scaleall * sctm;
    RenderShapeData mushstem = RenderShapeData{
        .primitive = mushstem_prim,
        .ctm = ctmShape,
        .ictm = sictm,
        .nictm = snictm,
        .ctmInitial = ctmShape
    };

    // make the mushroom
    mushroom = new MushroomData;
    mushroom->pieces = std::vector<RenderShapeData>{mushstem, mushtop};

    // extra hat!
    if(variant == 3)  {        
        glm::mat4 ctm_var4 = glm::mat4({cw/2, 0, 0, 0}, {0, ch, 0, 0}, {0, 0, cw/2, 0}, {xOffset, h + 0.8 * ch/2, zOffset, 1});
        glm::mat4 ictm_var4 = glm::inverse(ctm_var4);
        glm::mat4 nictm_var4 = glm::inverse(glm::transpose(glm::mat3(ctm_var4)));
        glm::mat4 ctmVar4 = scaleall * ctm_var4;

        RenderShapeData mushtop_var4 = RenderShapeData{
            .primitive = mushtop_prim,
            .ctm = ctmVar4,
            .ictm = ictm_var4,
            .nictm = nictm_var4,
            .ctmInitial = ctmVar4
        };

        mushroom->pieces.push_back(mushtop_var4);
    }

    mushroom->variant = variant;

    return mushroom;
}

std::vector<MushroomData*> SceneMaker::generateScene(int gridWidth, int gridHeight, int gridDistance) {
    std::vector<MushroomData*> mushGrid(gridWidth * gridHeight);
    std::uniform_real_distribution<> gridOffset(-gridDistance / 2.f, gridDistance / 2.f);

    int hgw = gridWidth / 2;
    int hgh = gridHeight / 2;

    for(int row = -hgh; row < hgh; row++) {
        for(int col = -hgw; col < hgw; col++) {
            mushGrid[(row + hgh) * gridWidth + (col + hgh)] = SceneMaker::generateMushroom(
                mushIdx(gen),
                gridDistance * col + gridOffset(gen),
                gridDistance * row + gridOffset(gen)
            );
        }
    }

    return mushGrid;
}

void SceneMaker::rotateMushroom(MushroomData* shroom, glm::vec4 look, float angle) {
    angle = glm::radians(angle);
    float c = cos(angle);
    float s = sin(angle);
    float x = 0;
    float y = 1;
    float z = 0;
    glm::mat4 rotate = glm::mat4({(c + x*x*(1-c)), (x*y*(1-c)+z*s), (x*z*(1-c) - y*s), 0},
                                 {(x*y*(1-c)+z*s), (c + y*y*(1-c)), (y*z*(1-c) - x*s), 0},
                                 {(x*z*(1-c)+y*s), (y*z*(1-c) - x*s), (c + z*z*(1-c)), 0},
                                 {0, 0, 0, 1});

    for (int i = 0; i < (shroom->pieces).size(); i++) {
        glm::mat4 oldctm = shroom->pieces[i].ctm;
        //float offZ = shroom->pieces[i].ctm[3][2];
        //glm::mat4 scale = glm::mat4({oldctm[0][0], 0, 0, 0}, {0, oldctm[1][1], 0, 0}, {0, 0, oldctm[2][2], 0}, {0, 0, 0, 1});
        //glm::mat4 detranslate = glm::mat4({1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {-shroom->xOff, -shroom->yOff, -shroom->zOff, 1});
        //glm::mat4 retranslate = glm::mat4({1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {shroom->xOff, shroom->yOff, shroom->zOff, 1});
        if(shroom->pieces[i].primitive.type == PrimitiveType::PRIMITIVE_MUSHTOP) {
            glm::mat4 newctm = oldctm * rotate;
            shroom->pieces[i].ctm = newctm;
            shroom->pieces[i].ictm = glm::inverse(newctm),
            shroom->pieces[i].nictm = glm::inverse(glm::transpose(newctm));
        }
    }
}

void SceneMaker::bounceMushroom(MushroomData* shroom, glm::vec4 look, float angle) {
    return;

    angle = glm::radians(angle);
    float c = cos(angle);
    float s = sin(angle);
    float x = 0;
    float y = 0;
    float z = 1;
    glm::mat4 rotate = glm::mat4({(c + x*x*(1-c)), (x*y*(1-c)+z*s), (x*z*(1-c) - y*s), 0},
                                 {(x*y*(1-c)+z*s), (c + y*y*(1-c)), (y*z*(1-c) - x*s), 0},
                                 {(x*z*(1-c)+y*s), (y*z*(1-c) - x*s), (c + z*z*(1-c)), 0},
                                 {0, 0, 0, 1});
    glm::mat4 sctm = shroom->pieces[0].ctm;
    for (int i = 0; i < (shroom->pieces).size(); i++) {
        glm::mat4 oldctm = shroom->pieces[i].ctm;
        glm::mat4 newctm;
        glm::mat4 up5 = glm::mat4({1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0.5, 0, 1});
        glm::mat4 down5 = glm::mat4({1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, -0.5, 0, 1});
        if(shroom->pieces[i].primitive.type == PrimitiveType::PRIMITIVE_CYLINDER) {
            newctm = oldctm * rotate;
        } else if(shroom->pieces[i].primitive.type == PrimitiveType::PRIMITIVE_MUSHTOP) {
            newctm = sctm * down5 * rotate * up5 * glm::inverse(sctm) * oldctm;

        }
        shroom->pieces[i].ctm = newctm;
        shroom->pieces[i].ictm = glm::inverse(newctm),
        shroom->pieces[i].nictm = glm::inverse(glm::transpose(newctm));
    }

        // sctm * translate down by 1 * rotate * translate up by 1 * sctm-1 * ctm
        // if stem
        // sctm * translate up by 0.5 * rotate

}

void SceneMaker::translateMushroom(MushroomData* shroom, float y) {
    glm::mat4 translate = glm::mat4({1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, y, 0, 1});
    for (int i = 0; i < (shroom->pieces).size(); i++) {
        glm::mat4 oldctm = shroom->pieces[i].ctm;
        glm::mat4 newctm = translate * oldctm;

        shroom->pieces[i].ctm = newctm;
        shroom->pieces[i].ictm = glm::inverse(newctm),
        shroom->pieces[i].nictm = glm::inverse(glm::transpose(newctm));
    }
}
