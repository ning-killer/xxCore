//
// Created by xiong on 2022/4/26.
//

#ifndef EMX_MediaRegion_HPP
#define EMX_MediaRegion_HPP
namespace Emx {

    //!@brief 图像大小
    struct Size {
        Size() : w(0), h(0) {}

        Size(int iw, int ih) : w(iw), h(ih) {}

        int w;  //!<图像宽
        int h; //!<图像高
    };

    //!@brief 矩形框
    struct Rect {
        Rect() : x(0), y(0), w(0), h(0) {}

        Rect(int ix, int iy, int iw, int ih) : x(ix), y(iy), w(iw), h(ih) {}

        /**
         * @brief 根据输入的原始画幅，将当前矩形框转换为基于目标画幅的矩形框
         * @param srcSize [in] 原始画幅大小
         * @param dstSize [in] 目标画幅大小
         * @return 目标矩形框
         */
        Rect Transfer(Size &srcSize, Size &dstSize);

        /**
         * @brief 计算当前矩形框面积
         * @return 当前矩形框面积
         */
        int Area() { return w * h; }

        int x;  //!<矩形框x坐标
        int y;  //!<矩形框y坐标
        int w;  //!<矩形框宽度
        int h;  //!<矩形框高度
    };

//    class RegionMask {
//    public:
//        const static int Width = 64;
//        const static int Height = 36;
//        struct Map {
//            bool block[Width * Height];
//        };
//
//        static bool HitMap(Size &srcSize, Rect &rect, Map &map);
//    };
}
#endif //EMX_MediaRegion_HPP
/*好好写代码，不然会被妈妈打
                                    ...vr
                                    qBMBBBMBMY
                                   8BBBBOBMRMV
                                 iMBMM5vOY:BMBBv
               .r,               OEM;    .:  rBBBBBY
               vUL               7EB    . :7.  LBMMEBM
              .@Wwz.            :uvir   . i: .iLMOMOBM .
               vv::r               iY.   ...rv,@arqiao.
                Li.i:               v: . ::::7vOBBMBL..
                ,i7: vSUi，         :M7.:.,:u08OP.  .
                  .N2k5ulju7,. .    :BMGiiL7   ,i,i
                   :rL jFYjvjLY7r::.  ;v  vr... rE8q;,:,,
                  751jSLYPFu5uUoguohez ou., 1wjr2E8@rizero.
                  BB:FMi rM8EGOPF jF 15F70Mu1 5F25Luu25Gi
               ivSvvXL   :v58ZOG ZXF2UUkF SFkU1u125uUUUZ,
             :@kewensun.     ,iY20GOXSUMkSuS2F5XXrkUM5SEv.
         .:iOBMBMBBOOBMUi;,       ,:8PkFP5NkPXkFFPEqokKZu.
      .r qMqBBMOMMBMBBBM.           @kexiarli. S11kFSU5g5
    .7BBOi 1L 1MMSEBBOMBB..,        8kg552kkuPP1Uokk1kUEJ
    .;MBZ; iiMBMBMMOBEBu,           10kkS1F1X5kFP112F51kU
      .rPY   OMEMEBEMEB2,.          rME5SSSFkXPGFNk SUPZ,
              ::JuBML::r:.:.,,       SZPXOSKSP5kYGNP 15UBr
                  L,  :@sanshao.     : MNZqNXqSqqXk2EoPSXPE
              viLBx.,,v8Bj.i:r7:,    2ZkqgOXXSNNONOLSKO
            :r2. rMGMGi   . TY,li::i vOOPWNNSIEqPoSecbone
            .ilr. .ikY.     vE.   iY.. 20Fq0g55F 1S2F22uw1M
 */
