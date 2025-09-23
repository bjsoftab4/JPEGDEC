import jpgdec_bin 


def getinfo(inbuf):
    res = jpgdec_bin.jpgdec_getinfo(inbuf)
    print(res)

def decode(inbuf, framebuf):
    res = jpgdec_bin.jpgdec_decode(inbuf, framebuf)
    print(res)
    return res
