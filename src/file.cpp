/*
    Copyright (C) 2009 Andrew Caudwell (acaudwell@gmail.com)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version
    3 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "file.h"
#include <iostream>

float gGourceFileDiameter  = 8.0;

std::vector<RFile*> gGourceRemovedFiles;

FXFont file_selected_font;
FXFont file_font;

RFile::RFile(const std::string & name, const vec3 & colour, const vec2 & pos, int tagid) : Pawn(name,pos,tagid) {
    hidden = true;
    size = gGourceFileDiameter * 1.05;
    radius = size * 0.5;

    setGraphic(gGourceSettings.file_graphic);

    speed = 5.0;
    nametime = gGourceSettings.filename_time;
    name_interval = nametime;

    namecol     = vec3(1.0, 1.0, 1.0);
    file_colour = colour;

    last_action    = 0.0f;
    fade_start     = -1.0f;
    removed_timestamp = 0;
    expired        = false;
    forced_removal = false;
    removing       = false;

    shadow = true;

    distance = 0;

    setFilename(name);
    colourize();

/*
    std::cerr << "Path '" << name << "' extension " << ext
              << " => colour "
              << file_colour[0] << ","
              << file_colour[1] << ","
              << file_colour[2]
              << std::endl;
*/
    
    if(!file_selected_font.initialized()) {
        file_selected_font = fontmanager.grab(gGourceSettings.font_file, 18);
        file_selected_font.dropShadow(true);
        file_selected_font.roundCoordinates(false);
        file_selected_font.setColour(vec4(gGourceSettings.selection_colour, 1.0f));
    }

    if(!file_font.initialized()) {
        file_font = fontmanager.grab(gGourceSettings.font_file, gGourceSettings.scaled_filename_font_size);
        file_font.dropShadow(true);
        file_font.roundCoordinates(false);
        file_font.setColour(vec4(gGourceSettings.filename_colour, 1.0f));
    }

    setSelected(false);

    dir = 0;
}

RFile::~RFile() {
}

void RFile::remove(time_t removed_timestamp) {
    last_action = elapsed;
    fade_start  = elapsed;
    removing = true;
    this->removed_timestamp = removed_timestamp;
}

void RFile::remove() {
    forced_removal = true;
    remove(0);
}

void RFile::setDir(RDirNode* dir) {
    this->dir = dir;
}

RDirNode* RFile::getDir() const{
    return dir;
}

vec2 RFile::getAbsolutePos() const{
    return pos + dir->getPos();
}

bool RFile::overlaps(const vec2& pos) const {

    vec2 abs_pos = getAbsolutePos();

    float halfsize_x = size * 0.5f;
    vec2 halfsize ( halfsize_x, halfsize_x * graphic_ratio );

    Bounds2D file_bounds(abs_pos - halfsize, abs_pos + halfsize);

    return file_bounds.contains(pos);
}

void RFile::setFilename(const std::string& abs_file_path) {

    fullpath = abs_file_path;

    size_t pos = fullpath.rfind('/');

    if(pos != std::string::npos) {
        path = name.substr(0,pos+1);
        name = name.substr(pos+1, std::string::npos);
    } else {
        path = std::string("");
        name = abs_file_path;
    }

    //trim name to just extension
    size_t dotsep = name.rfind(".");
    size_t filetype = name.rfind(",");
    if (filetype == name.size() - 4 &&
        std::isxdigit(name[filetype + 1]) &&
        std::isxdigit(name[filetype + 2]) &&
        std::isxdigit(name[filetype + 3])) {

        // Try to convert the type number to a name
        std::string type = name.substr(filetype + 1);
        if (type == "fff") { ext = std::string("type Text"); }
        else if (type == "ffe") { ext = std::string("type Command"); }
        else if (type == "ffd") { ext = std::string("type Data"); }
        else if (type == "ffc") { ext = std::string("type Utility"); }
        else if (type == "ffb") { ext = std::string("type BASIC"); }
        else if (type == "ffa") { ext = std::string("type Module"); }
        else if (type == "ff9") { ext = std::string("type Sprite"); }
        else if (type == "ff8") { ext = std::string("type Absolute"); }
        else if (type == "ff7") { ext = std::string("type BBC font"); }
        else if (type == "ff6") { ext = std::string("type Font"); }
        else if (type == "ff5") { ext = std::string("type PoScript"); }
        else if (type == "ff4") { ext = std::string("type Printout"); }
        else if (type == "ff2") { ext = std::string("type Config"); }
        else if (type == "ff0") { ext = std::string("type TIFF"); }
        else if (type == "fd1") { ext = std::string("type BasicTxt"); }
        else if (type == "fed") { ext = std::string("type Palette"); }
        else if (type == "fec") { ext = std::string("type Template"); }
        else if (type == "feb") { ext = std::string("type Obey"); }
        else if (type == "fea") { ext = std::string("type Desktop"); }
        else if (type == "fe6") { ext = std::string("type Unix Ex"); }
        else if (type == "fe5") { ext = std::string("type EPROM"); }
        else if (type == "fdc") { ext = std::string("type SoftLink"); }
        else if (type == "fd3") { ext = std::string("type DebImage"); }
        else if (type == "fca") { ext = std::string("type Squash"); }
        else if (type == "fc9") { ext = std::string("type SunRastr"); }
        else if (type == "faf") { ext = std::string("type HTML"); }
        else if (type == "fae") { ext = std::string("type Resource"); }
        else if (type == "f89") { ext = std::string("type GZip"); }
        else if (type == "d94") { ext = std::string("type ArtWork"); }
        else if (type == "c85") { ext = std::string("type JPEG"); }
        else if (type == "bbc") { ext = std::string("type BBC ROM"); }
        else if (type == "b61") { ext = std::string("type XBM"); }
        else if (type == "b60") { ext = std::string("type PNG"); }
        else if (type == "b2f") { ext = std::string("type WMF"); }
        else if (type == "aff") { ext = std::string("type DrawFile"); }
        else if (type == "a91") { ext = std::string("type Zip"); }
        else if (type == "a66") { ext = std::string("type WebP"); }
        else if (type == "a65") { ext = std::string("type JPEG2000"); }
        else if (type == "69e") { ext = std::string("type PNM"); }
        else if (type == "69d") { ext = std::string("type Targa"); }
        else if (type == "69c") { ext = std::string("type BMP"); }
        else if (type == "697") { ext = std::string("type PCX"); }
        else if (type == "695") { ext = std::string("type GIF"); }
        else if (type == "690") { ext = std::string("type Clear"); }
        else if (type == "1c9") { ext = std::string("type DiagData"); }
        else if (type == "132") { ext = std::string("type ICO"); }
        else {
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "type &%s", type.c_str());
            ext = std::string(buffer);
        }

        /*
        vec3 ch = colourHash(ext);
        std::cerr << "Filetype name '" << buffer << "' => "
                  << ch[0] << ","
                  << ch[1] << ","
                  << ch[2]
                  << std::endl;
        */
    }

    // Special case for the RISC OS Pyromaniac source code wth untyped files
    else if (fullpath.find("/o/") != std::string::npos) {
        ext = std::string("AOF");
    }
    else if (fullpath.find("/s/") != std::string::npos ||
             fullpath.find("/hdr/") != std::string::npos) {
        ext = std::string("ObjAsm");
    }
    else if (fullpath.find("/bin/") != std::string::npos) {
        // These are always bare utilities if they haven't got a filetype on the end.
        ext = std::string("type Utility");
    }
    else if (fullpath.find("/expect/") != std::string::npos) {
        // All the files in the expectation directory are text files.
        ext = std::string("expectation");
    }
    else if (fullpath.find("testcode/tests") != std::string::npos) {
        // All the files in the expectation directory are text files.
        ext = std::string("test script");
    }

    else if(dotsep != std::string::npos && dotsep != name.size()-1) {
        ext = name.substr(dotsep+1);
    } else if(gGourceSettings.file_extension_fallback) {
        ext = name;
    }
}

void RFile::colourize() {
    //file_colour = vec3(0.0f, 0.0f, 1.0f);
    file_colour = ext.size() ? colourHash(ext) : vec3(1.0f, 1.0f, 1.0f);
}

const vec3& RFile::getNameColour() const{
    return selected ? gGourceSettings.selection_colour : namecol;
}

void RFile::setFileColour(const vec3 & colour) {
    file_colour = colour;
}

const vec3 & RFile::getFileColour() const{
    return file_colour;
}

vec3 RFile::getColour() const{

    if(selected) return vec3(1.0f);

    float lc = elapsed - last_action;

    if(lc<1.0f) {
        return touch_colour * (1.0f-lc) + file_colour * lc;
    }

    return file_colour;
}

float RFile::getAlpha() const{
    float alpha = Pawn::getAlpha();

    //user fades out if not doing anything
    if(fade_start > 0.0f) {
        alpha = 1.0 - glm::clamp(elapsed - fade_start, 0.0f, 1.0f);
    }

    return alpha;
}

void RFile::logic(float dt) {
    Pawn::logic(dt);

    vec2 dest_pos = dest;
/*
    if(dir->getParent() != 0 && dir->noDirs()) {
        vec2 dirnorm = dir->getNodeNormal();
        dest_pos = dirnorm + dest;
    }*/

    dest_pos = dest_pos * distance;

    accel = dest_pos - pos;

    // apply accel
    vec2 accel2 = accel * speed * dt;

    if(glm::length2(accel2) > glm::length2(accel)) {
        accel2 = accel;
    }

    pos += accel2;

    //files have no momentum
    accel = vec2(0.0f, 0.0f);

    if(fade_start < 0.0f && gGourceSettings.file_idle_time > 0.0f && (elapsed - last_action) > gGourceSettings.file_idle_time) {
        fade_start = elapsed;
    }
    
    // has completely faded out
    if(fade_start > 0.0f && !expired && (elapsed - fade_start) >= 1.0) {

        expired = true;

        bool found = false;
        for(std::vector<RFile*>::iterator it = gGourceRemovedFiles.begin(); it != gGourceRemovedFiles.end(); it++) {
            if((*it) == this) {
                found = true;
                break;
            }
        }

        if(!found) {
            gGourceRemovedFiles.push_back(this);
            //fprintf(stderr, "expiring %s\n", fullpath.c_str());
        }
    }

    if(isHidden() && !forced_removal) elapsed = 0.0;
}

void RFile::touch(time_t touched_timestamp, const vec3 & colour) {
    if(forced_removal || (removing && touched_timestamp < removed_timestamp)) return;

    //fprintf(stderr, "touch %s\n", fullpath.c_str());

    fade_start = -1.0f;
    removing = false;
    removed_timestamp = 0;
    last_action = elapsed;
    touch_colour = colour;

    //un expire file if touched after being removed
    if(expired) {
        for(std::vector<RFile*>::iterator it = gGourceRemovedFiles.begin(); it != gGourceRemovedFiles.end(); it++) {
            if((*it) == this) {
                gGourceRemovedFiles.erase(it);
                break;
            }
        }
        expired=false;
    }

    showName();
    setHidden(false);
    dir->fileUpdated(true);
}

void RFile::setHidden(bool hidden) {
    if(this->hidden==true && hidden==false && dir !=0) {
        dir->addVisible();
    }

    Pawn::setHidden(hidden);
}

void RFile::calcScreenPos(GLint* viewport, GLdouble* modelview, GLdouble* projection) {

    static GLdouble screen_x, screen_y, screen_z;

    vec2 text_pos = getAbsolutePos();
    text_pos.x += 5.5f;

    if(selected)
        text_pos.y -= 2.0f;
    else
        text_pos.y -= 1.0f;

    gluProject( text_pos.x, text_pos.y, 0.0f, modelview, projection, viewport, &screen_x, &screen_y, &screen_z);
    screen_y = (float)viewport[3] - screen_y;

    screenpos.x = screen_x;
    screenpos.y = screen_y;
}

void RFile::drawNameText(float alpha) {
    if(!selected && alpha <= 0.01) return;

    float name_alpha = selected ? 1.0 : alpha;

    if(selected) {
        file_selected_font.draw(screenpos.x, screenpos.y, name);
    } else {
        file_font.setAlpha(name_alpha);
        file_font.draw(screenpos.x, screenpos.y, gGourceSettings.file_extensions ? ext : name);
    }
}

void RFile::draw(float dt) {
    Pawn::draw(dt);

    glLoadName(0);
}
