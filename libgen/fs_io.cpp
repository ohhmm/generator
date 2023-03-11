#include "fs_io.hpp"

#include <boost/gil.hpp>
#include <boost/gil/extension/io/bmp.hpp>
#include <boost/gil/extension/io/targa.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/extension/io/jpeg.hpp>

using namespace ::gen::io::fs;

bool Save(const ::boost::gil::rgba8_image_t& img, const ::boost::filesystem::path& filepath)
{
    bool success = true;
    
    auto ext = filepath.extension();
    if(ext.empty()){
        auto pngFilePath = filepath;
        pngFilePath.replace_extension(".png");
        success = Save(img, pngFilePath);
    }
    else {
        auto v = boost::gil::const_view(img);
        try{
            std::ofstream file(filepath.c_str(), std::ios_base::out | std::ios_base::binary);
            if (ext == ".bmp") {
                boost::gil::write_view(file, v, boost::gil::bmp_tag());
            }
            else if (ext == ".png") {
                boost::gil::write_view(file, v, boost::gil::png_tag());
            }
            else if (ext == ".jpg" || ext == ".jpeg") {
                boost::gil::rgb8_image_t jpgImg(v.dimensions());
                auto jpgView = boost::gil::view(jpgImg);
                copy_and_convert_pixels(v, jpgView); 
                boost::gil::write_view(file, jpgView, boost::gil::jpeg_tag());
            }
            else if(ext == ".tga"){
                boost::gil::write_view(file, v, boost::gil::targa_tag());
            }
            else {
                success = {};
            }
            file.close();
        }catch(...){
            success = {};
        }
    }
    return success;
}
