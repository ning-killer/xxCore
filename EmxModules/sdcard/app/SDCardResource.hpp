//
// Created by xiong on 2022/6/13.
//

#ifndef EMX_SDCardResource_HPP
#define EMX_SDCardResource_HPP
namespace Emx {
    class SDCardResource {
    public:
        EuvLoop loop;
        MsgTopicPublisherAsync pub;
        char *buffer;
        int size;
    };
}
#endif //EMX_SDCardResource_HPP

