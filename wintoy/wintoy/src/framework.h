#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__

int GetFPS();

void InitFramework(int width, int height, void* window);
void UpdateFramework();
void EndFramework();

bool TouchBegin(int x, int y);
void TouchMove(int x, int y);
void TouchEnd(int x, int y);

void TouchNext();
void TouchBack();

#endif
