#ifndef __INITRAMFS_H__
#define __INITRAMFS_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The function parses initrd/initramfs image and populates
 *  ramfs with content of the image.
 * 
 */
void initramfs_setup(void);

#ifdef __cplusplus
}
#endif

#endif