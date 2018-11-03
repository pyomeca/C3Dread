#define EZC3D_API_EXPORTS
///
/// \file Analogs.cpp
/// \brief Implementation of Analogs class
/// \author Pariterre
/// \version 1.0
/// \date October 17th, 2018
///

#include "Analogs.h"

ezc3d::DataNS::AnalogsNS::Analogs::Analogs()
{

}

ezc3d::DataNS::AnalogsNS::Analogs::Analogs(size_t nbSubframes)
{
    _subframe.resize(nbSubframes);
}

void ezc3d::DataNS::AnalogsNS::Analogs::print() const
{
    for (size_t i = 0; i < nbSubframes(); ++i){
        std::cout << "Subframe = " << i << std::endl;
        subframe(i).print();
        std::cout << std::endl;
    }
}

void ezc3d::DataNS::AnalogsNS::Analogs::write(std::fstream &f) const
{
    for (size_t i = 0; i < nbSubframes(); ++i){
        subframe(i).write(f);
    }
}

size_t ezc3d::DataNS::AnalogsNS::Analogs::nbSubframes() const
{
    return _subframe.size();
}

const ezc3d::DataNS::AnalogsNS::SubFrame& ezc3d::DataNS::AnalogsNS::Analogs::subframe(size_t idx) const
{
    try {
        return _subframe.at(idx);
    } catch(std::out_of_range) {
        throw std::out_of_range("Analogs::subframe method is trying to access the subframe "
                                + std::to_string(idx) +
                                " while the maximum number of subframes is "
                                + std::to_string(nbSubframes()) + ".");
    }
}

ezc3d::DataNS::AnalogsNS::SubFrame &ezc3d::DataNS::AnalogsNS::Analogs::subframe_nonConst(size_t idx)
{
    try {
        return _subframe.at(idx);
    } catch(std::out_of_range) {
        throw std::out_of_range("Analogs::subframe method is trying to access the subframe "
                                + std::to_string(idx) +
                                " while the maximum number of subframes is "
                                + std::to_string(nbSubframes()) + ".");
    }
}

void ezc3d::DataNS::AnalogsNS::Analogs::subframe(const ezc3d::DataNS::AnalogsNS::SubFrame& subframe, size_t idx)
{
    if (idx == SIZE_MAX)
        _subframe.push_back(subframe);
    else{
        if (idx >= nbSubframes())
            _subframe.resize(idx+1);
        _subframe[idx] = subframe;
    }
}

const std::vector<ezc3d::DataNS::AnalogsNS::SubFrame> &ezc3d::DataNS::AnalogsNS::Analogs::subframes() const
{
    return _subframe;
}
