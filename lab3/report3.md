其实，cdev_init()和cdev_alloc()的功能是差不多的，只是前者多了一个ops的赋值操作，
具体区别参看下面两个函数的实现代码：
struct cdev *cdev_alloc(void)
{
struct cdev *p = kzalloc(sizeof(struct cdev), GFP_KERNEL);
if (p) {
INIT_LIST_HEAD(&p->list);
kobject_init(&p->kobj, &ktype_cdev_dynamic);
}
return p;
}
void cdev_init(struct cdev *cdev, const struct file_operations *fops)
{
memset(cdev, 0, sizeof *cdev);
INIT_LIST_HEAD(&cdev->list);
kobject_init(&cdev->kobj, &ktype_cdev_default);
cdev->ops = fops;
}
两个函数的原型定义在/usr/src/linux-4.4.19/include/linux/cdev.h 文件中。


cdev 初始化完成后，应将其注册到系统中，一般在模块加载时完成该操作。设备注册
函数是 cdev_add()，其原型定义在/usr/src/linux-4.4.19/include/linux/cdev.h 文件中：
int cdev_add(struct cdev *p, dev_t dev, unsigned count)
{
p->dev = dev;
p->count = count;
return kobj_map(cdev_map, dev, count, NULL, exact_match, exact_lock, p);
}
其中的输入参数分别是 cdev 结构指针、起始设备号、次设备号数量


chmod 777 dev