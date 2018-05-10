#include "Data.h"
// Implementation of data class


ezC3D::DataNS::Data::Data(ezC3D::C3D &file)
{
    // Firstly read a dummy value just prior to the data so it moves the pointer to the right place
    file.readInt(ezC3D::READ_SIZE::BYTE, 256*ezC3D::READ_SIZE::WORD*(file.header().parametersAddress()-1) + 256*ezC3D::READ_SIZE::WORD*file.parameters().nbParamBlock() - ezC3D::READ_SIZE::BYTE, std::ios::beg); // "- BYTE" so it is just prior

    // Read the actual data
    for (int j = 0; j < file.header().nbFrames(); ++j){
        ezC3D::DataNS::Frame frame;

        // Get names of the data
        std::vector<std::string> pointNames(file.parameters().group("POINT").parameter("LABELS").stringValues());
        std::vector<std::string> analogNames(file.parameters().group("ANALOG").parameter("LABELS").stringValues());

        // Read point 3d
        if (file.header().scaleFactor() < 0){
            ezC3D::DataNS::Points3dNS::Points3d pts;
            for (int i = 0; i < file.header().nb3dPoints(); ++i){
                ezC3D::DataNS::Points3dNS::Point p;
                p.x(file.readFloat());
                p.y(file.readFloat());
                p.z(file.readFloat());
                p.residual(file.readFloat());
                if (i < pointNames.size())
                    p.name(pointNames[i]);
                else
                    p.name("unlabeled_point_" + i);
                pts.add(p);
            }
            frame.add(pts);

            ezC3D::DataNS::AnalogsNS::Analogs analog;
            for (int k = 0; k < file.header().nbAnalogByFrame(); ++k){
                ezC3D::DataNS::AnalogsNS::SubFrame sub;
                for (int i = 0; i < file.header().nbAnalogs(); ++i){
                    ezC3D::DataNS::AnalogsNS::Channel c;
                    c.value(file.readFloat());
                    if (i < pointNames.size())
                        c.name(analogNames[i]);
                    else
                        c.name("unlabeled_analog_" + i);
                    sub.addChannel(c);
                }
                analog.addSubframe(sub);
            }
            frame.add(analog);
        }
        else
            throw std::invalid_argument("Points were recorded using int number which is not implemented yet");

        _frames.push_back(frame);
    }

}
const std::vector<ezC3D::DataNS::Frame>& ezC3D::DataNS::Data::frames() const
{
    return _frames;
}
const ezC3D::DataNS::Frame& ezC3D::DataNS::Data::frame(int idx) const
{
    if (idx < 0 || idx >= frames().size())
        throw std::out_of_range("Wrong number of frames");
    return _frames[idx];
}
void ezC3D::DataNS::Data::print() const
{
    for (int i = 0; i < frames().size(); ++i){
        std::cout << "Frame " << i << std::endl;
        frame(i).print();
        std::cout << std::endl;
    }
}



// Point3d data
void ezC3D::DataNS::Points3dNS::Points3d::add(ezC3D::DataNS::Points3dNS::Point p)
{
    _points.push_back(p);
}
void ezC3D::DataNS::Points3dNS::Points3d::print() const
{
    for (int i = 0; i < points().size(); ++i)
        point(i).print();
}

const std::vector<ezC3D::DataNS::Points3dNS::Point>& ezC3D::DataNS::Points3dNS::Points3d::points() const
{
    return _points;
}

const ezC3D::DataNS::Points3dNS::Point& ezC3D::DataNS::Points3dNS::Points3d::point(int idx) const
{
    if (idx < 0 || idx >= points().size())
        throw std::out_of_range("Tried to access wrong index for points data");
    return _points[idx];
}
const ezC3D::DataNS::Points3dNS::Point &ezC3D::DataNS::Points3dNS::Points3d::point(const std::string &pointName) const
{
    for (int i = 0; i<points().size(); ++i)
        if (!point(i).name().compare(pointName))
            return point(i);
    throw std::invalid_argument("Point name was not found within the points");
}
void ezC3D::DataNS::Points3dNS::Point::print() const
{
    std::cout << name() << " = [" << x() << ", " << y() << ", " << z() << "]; Residual = " << residual() << std::endl;
}



ezC3D::DataNS::Points3dNS::Point::Point() :
    _name(""),
    _data(new float[4]())
{

}
float ezC3D::DataNS::Points3dNS::Point::x() const
{
    return _data[0];
}
void ezC3D::DataNS::Points3dNS::Point::x(float x)
{
    _data[0] = x;
}

float ezC3D::DataNS::Points3dNS::Point::y() const
{
    return _data[1];
}
void ezC3D::DataNS::Points3dNS::Point::y(float y)
{
    _data[1] = y;
}

float ezC3D::DataNS::Points3dNS::Point::z() const
{
    return _data[2];
}
void ezC3D::DataNS::Points3dNS::Point::z(float z)
{
    _data[2] = z;
}
float ezC3D::DataNS::Points3dNS::Point::residual() const
{
    return _data[3];
}
void ezC3D::DataNS::Points3dNS::Point::residual(float residual){
    _data[3] = residual;
}
const std::string& ezC3D::DataNS::Points3dNS::Point::name() const
{
    return _name;
}
void ezC3D::DataNS::Points3dNS::Point::name(const std::string &name)
{
    std::string name_copy = name;
    // Remove the spaces at the end of the name
    for (int i = name_copy.size(); i >= 0; --i)
        if (name_copy[name_copy.size()-1] == ' ')
            name_copy.pop_back();
        else
            break;
    _name = name_copy;
}




// Analog data
void ezC3D::DataNS::AnalogsNS::SubFrame::print() const
{
    for (int i = 0; i < channels().size(); ++i){
        channel(i).print();
    }
}

void ezC3D::DataNS::AnalogsNS::SubFrame::addChannel(ezC3D::DataNS::AnalogsNS::Channel channel)
{
    _channels.push_back(channel);
}
void ezC3D::DataNS::AnalogsNS::SubFrame::addChannels(const std::vector<ezC3D::DataNS::AnalogsNS::Channel>& allChannelsData)
{
    _channels = allChannelsData;
}
const std::vector<ezC3D::DataNS::AnalogsNS::Channel>& ezC3D::DataNS::AnalogsNS::SubFrame::channels() const
{
    return _channels;
}
const ezC3D::DataNS::AnalogsNS::Channel &ezC3D::DataNS::AnalogsNS::SubFrame::channel(int idx) const
{
    if (idx < 0 || idx >= _channels.size())
        throw std::out_of_range("Tried to access wrong index for analog data");
    return _channels[idx];
}
const ezC3D::DataNS::AnalogsNS::Channel &ezC3D::DataNS::AnalogsNS::SubFrame::channel(std::string channelName) const
{
    for (int i = 0; i < channels().size(); ++i)
        if (!channel(i).name().compare(channelName))
            return channel(i);
    throw std::invalid_argument("Analog name was not found within the analogs");
}
float ezC3D::DataNS::AnalogsNS::Channel::value() const
{
    return _value;
}
void ezC3D::DataNS::AnalogsNS::Channel::value(float v)
{
    _value = v;
}
void ezC3D::DataNS::AnalogsNS::Channel::print() const
{
    std::cout << "Analog[" << name() << "] = " << value() << std::endl;
}

const std::string& ezC3D::DataNS::AnalogsNS::Channel::name() const
{
    return _name;
}

void ezC3D::DataNS::AnalogsNS::Channel::name(const std::string &name)
{
    std::string name_copy = name;
    // Remove the spaces at the end of the name
    for (int i = name_copy.size(); i >= 0; --i)
        if (name_copy[name_copy.size()-1] == ' ')
            name_copy.pop_back();
        else
            break;

    _name = name_copy;
}




// Frame data
void ezC3D::DataNS::Frame::print() const
{
    points().print();
    analogs().print();
}
const ezC3D::DataNS::Points3dNS::Points3d& ezC3D::DataNS::Frame::points() const
{
    return *_points;
}
const ezC3D::DataNS::AnalogsNS::Analogs& ezC3D::DataNS::Frame::analogs() const
{
    return *_analogs;
}
void ezC3D::DataNS::Frame::add(ezC3D::DataNS::AnalogsNS::Analogs analogs_frame)
{
    _analogs = std::shared_ptr<ezC3D::DataNS::AnalogsNS::Analogs>(new ezC3D::DataNS::AnalogsNS::Analogs(analogs_frame));
}
void ezC3D::DataNS::Frame::add(ezC3D::DataNS::Points3dNS::Points3d point3d_frame)
{
    _points = std::shared_ptr<ezC3D::DataNS::Points3dNS::Points3d>(new ezC3D::DataNS::Points3dNS::Points3d(point3d_frame));
}
void ezC3D::DataNS::Frame::add(ezC3D::DataNS::Points3dNS::Points3d point3d_frame, ezC3D::DataNS::AnalogsNS::Analogs analog_frame)
{
    add(point3d_frame);
    add(analog_frame);
}
const std::vector<ezC3D::DataNS::AnalogsNS::SubFrame>& ezC3D::DataNS::AnalogsNS::Analogs::subframes() const
{
    return _subframe;
}
const ezC3D::DataNS::AnalogsNS::SubFrame& ezC3D::DataNS::AnalogsNS::Analogs::subframe(int idx) const
{
    if (idx < 0 || idx >= _subframe.size())
        throw std::out_of_range("Tried to access wrong subframe index for analog data");
    return _subframe[idx];
}
void ezC3D::DataNS::AnalogsNS::Analogs::print() const
{
    for (int i = 0; i < subframes().size(); ++i){
        std::cout << "Subframe = " << i << std::endl;
        subframe(i).print();
        std::cout << std::endl;
    }
}
void ezC3D::DataNS::AnalogsNS::Analogs::addSubframe(const ezC3D::DataNS::AnalogsNS::SubFrame& subframe)
{
    _subframe.push_back(subframe);
}
