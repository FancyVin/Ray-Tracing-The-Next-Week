//*************************************
// book2(RT in 1 week) ch10 Final
// �����ԭ�� ������¹��ܣ�
// *(ch01&02) �޸��˽���д����岻�ᷴ���˶�ģ���� bug, in material.h
// *(ch03) �޸����� hitbale->hittable  ԭ��book1Ӧ����ƴ���� �ڶ������Ѿ��޸�Ϊhittable
// *(ch03) ԭ����û�и���main.cpp���޸����ݣ���random_scene()���������һ��
// *(ch03) ���ͳ�Ƴ�������ʱ�书�� ʹ��bvhǰ:370s  ʹ�ú�:117s
// *(ch04) �޸�����ͼ����ɫΪ HuaweiP30Pro�����ͬ����ɫ
//*************************************

// stbͼ��� from https://github.com/nothings/stb.git
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <time.h>

#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "moving_sphere.h"
#include "bvh.h"
#include "texture.h"
#include "rect.h"
#include "flip_normal.h"
#include "box.h"
#include "instance.h"
#include "volume.h"

vec3 color(const ray& r, hittable *world, int depth) {
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec)) {
		ray scattered;
		vec3 attenuation;
		vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return emitted + attenuation * color(scattered, world, depth + 1);
		}
		else {
			return emitted;
		}
	} 
	else {
		return vec3(0, 0, 0);
	}
}

//scene final
hittable *final() {
	int nb = 20;
	hittable **list = new hittable*[30];
	hittable **boxlist = new hittable*[10000];
	hittable **boxlist2 = new hittable*[10000];
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *ground = new lambertian(new constant_texture(vec3(0.48, 0.83, 0.53)));
	int b = 0;
	for (int i = 0; i < nb; i++) {
		for (int j = 0; j < nb; j++) {
			float w = 100;
			float x0 = -1000 + i * w;
			float z0 = -1000 + j * w;
			float y0 = 0;
			float x1 = x0 + w;
			float y1 = 100 * ((rand() % 100 / float(100)) + 0.01);
			float z1 = z0 + w;
			boxlist[b++] = new box(vec3(x0, y0, z0), vec3(x1, y1, z1), ground);
		}
	}
	int l = 0;
	list[l++] = new bvh_node(boxlist, b, 0, 1);
	material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));
	list[l++] = new xz_rect(123, 423, 147, 412, 554, light);
	vec3 center(400, 400, 200);
	list[l++] = new moving_sphere(center, center + vec3(30, 0, 0),
		0, 1, 50, new lambertian(new constant_texture(vec3(0.7, 0.3, 0.1))));
	list[l++] = new sphere(vec3(260, 150, 45), 50, new dielectric(1.5));
	list[l++] = new sphere(vec3(0, 150, 145), 50,
		new metal(vec3(0.8, 0.8, 0.9), 10.0));
	hittable *boundary = new sphere(vec3(360, 150, 145), 70, new dielectric(1.5));
	list[l++] = boundary;
	list[l++] = new constant_medium(boundary, 0.2,
		new constant_texture(vec3(0.2, 0.4, 0.9)));
	boundary = new sphere(vec3(0, 0, 0), 5000, new dielectric(1.5));
	list[l++] = new constant_medium(boundary, 0.0001,
		new constant_texture(vec3(1.0, 1.0, 1.0)));
	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("earthmap.jpg", &nx, &ny, &nn, 0);
	material *emat = new lambertian(new image_texture(tex_data, nx, ny));
	list[l++] = new sphere(vec3(400, 200, 400), 100, emat);
	texture *pertext = new noise_texture(0.1);
	list[l++] = new sphere(vec3(220, 280, 300), 80, new lambertian(pertext));
	int ns = 1000;
	for (int j = 0; j < ns; j++) {
		boxlist2[j] = new sphere(
			vec3(165 * (rand() % 100 / float(100)), 165 * (rand() % 100 / float(100)), 165 * (rand() % 100 / float(100))),
			10, white);
	}
	list[l++] = new translate(new rotate_y(
		new bvh_node(boxlist2, ns, 0.0, 1.0), 15), vec3(-100, 270, 395));
	return new hittable_list(list, l);
}

//scene volume
hittable *cornell_smoke() {
	hittable **list = new hittable*[8];
	int i = 0;
	material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));

	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(113, 443, 127, 432, 554, light);
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));

	hittable *b1 = new translate(
		new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18),
		vec3(130, 0, 65));
	hittable *b2 = new translate(
		new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15),
		vec3(265, 0, 295));

	list[i++] = new constant_medium(
		b1, 0.01, new constant_texture(vec3(1.0, 1.0, 1.0)));
	list[i++] = new constant_medium(
		b2, 0.01, new constant_texture(vec3(0.0, 0.0, 0.0)));

	return new bvh_node(list, i, 0.0, 1.0);
}

//scene rec and light
hittable* cornell_box() {
	hittable **list = new hittable*[6];
	int i = 0;
	material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));

	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
	list[i++] = new translate(
		new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18),
		vec3(130, 0, 65)
	);
	list[i++] = new translate(
		new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15),
		vec3(265, 0, 295)
	);

	return new bvh_node(list, i, 0.0f, 1.0f);
}

//scene rec and light
hittable* simple_light() {
	texture *pertext = new noise_texture(4);
	hittable **list = new hittable*[4];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
	list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
	list[2] = new sphere(vec3(0, 7, 0), 2,
		new diffuse_light(new constant_texture(vec3(4, 4, 4))));
	list[3] = new xy_rect(3, 5, 1, 3, -2,
		new diffuse_light(new constant_texture(vec3(4, 4, 4))));
	return new bvh_node(list, 4, 0.0f, 1.0f);
}

//scene perlin noise
hittable* two_perlin_spheres() {
	texture *pertext = new noise_texture(4);
	hittable **list = new hittable*[2];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
	list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
	//return new hittable_list(list, 2);
	return new bvh_node(list, 2, 0.0f, 1.0f);
}

// scene solid textrue
hittable* two_spheres() {
	texture *checker_1 = new checker_texture(
		new constant_texture(vec3(1.0f, 0.31f, 0.0f)),
		new constant_texture(vec3(0.9, 0.9, 0.9))
	);
	texture *checker_2 = new checker_texture(
		new constant_texture(vec3(0.0f, 1.0f, 0.8f)),
		new constant_texture(vec3(0.9, 0.9, 0.9))
	);
	int n = 50;
	hittable **list = new hittable*[n + 1];
	list[0] = new sphere(vec3(0, -10, 0), 10, new lambertian(checker_1));
	list[1] = new sphere(vec3(0, 10, 0), 10, new lambertian(checker_2));
	return new bvh_node(list, 2, 0.0f, 1.0f);
}

// scene solid textrue & image texture
hittable* random_scene()
{
	int n = 500;
	hittable **list = new hittable *[n + 1];
	texture *checker = new checker_texture(
		new constant_texture(vec3(1.0f, 0.31f, 0.0f)),
		new constant_texture(vec3(0.9f, 0.9f, 0.9f))
	);
	list[0] = new sphere(vec3(0.0f, -1000.0f, 0.0f), 1000.0f, new lambertian(checker));
	int i = 1;
	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			float choose_mat = (rand() % 100 / float(100));
			vec3 center(a + 0.9f*(rand() % 100 / float(100)), 0.2f, b + 0.9f*(rand() % 100 / float(100)));
			if ((center - vec3(4.0f, 0.2f, 0.0f)).length() > 0.9f)
			{
				if (choose_mat < 0.8f)
				{
					list[i++] = new moving_sphere(center, center+vec3(0, (rand() % 100 / float(100))*0.5, 0), 0.0, 1.0, 0.2f,
						new lambertian(new constant_texture(vec3((rand() % 100 / float(100)) * (rand() % 100 / float(100)), (rand() % 100 / float(100)) * (rand() % 100 / float(100)), (rand() % 100 / float(100)) * (rand() % 100 / float(100))))));
				}
				else if (choose_mat < 0.95f)
				{
					list[i++] = new sphere(center, 0.2f,
						new metal(vec3(0.5f * (1 + (rand() % 100 / float(100))), 0.5f * (1 + (rand() % 100 / float(100))), 0.5f * (1 + (rand() % 100 / float(100)))), 0.5f * (rand() % 100 / float(100))));
				}
				else
				{
					list[i++] = new sphere(center, 0.2f, new dielectric(1.5f));
				}
			}
		}
	}

	// image texture
	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("earthmap.jpg", &nx, &ny, &nn, 0);

	list[i++] = new sphere(vec3(0.0f, 1.0f, 0.0f), 1.0f, new dielectric(1.5f));
	list[i++] = new sphere(vec3(-4.0f, 1.0f, 0.0f), 1.0f, new metal(vec3(0.7f, 0.6f, 0.5f), 0.0f));
	list[i++] = new sphere(vec3(4.0f, 1.0f, 0.0f), 1.0f, new lambertian(new image_texture(tex_data, nx, ny)));
	//return new hittable_list(list, i);
	return new bvh_node(list, i, 0.0f, 1.0f); //ʹ��bvh
}


int main() {
	// ͳ�Ƴ�������ʱ��
	clock_t start_time, end_time;
	start_time = clock();
	
	// width, height, channels of image
	int nx = 1000; // width
	int ny = 1000; // height
	int channels = 3;
	int ns = 64; // sample count

	// �洢ͼ������
	unsigned char *data = new unsigned char[nx*ny*channels];
	
	// �������
	hittable *world = final();
	// camera
	//vec3 lookfrom(278, 278, -800);
	vec3 lookfrom(478, 278, -600);
	vec3 lookat(278, 278, 0);
	float dist_to_focus = 10.0;
	float aperture = 0.0;
	float vfov = 40.0;

	camera cam(lookfrom, lookat, vec3(0, 1, 0), vfov, float(nx) / float(ny),
		aperture, dist_to_focus, 0.0, 1.0);

	// ѭ������ͼ��nx*ny�е�ÿ������
	for (int j = ny - 1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			vec3 col(0,0,0);
			// ���� ns ��
			for (int k = 0; k < ns; k++) {
				float u = float(i + (rand() % 100 / float(100))) / float(nx);
				float v = float(j + (rand() % 100 / float(100))) / float(ny);
				// ȷ�� ray r
				ray r = cam.get_ray(u, v);
				// �ۼ� ray r ���볡�� world �󣬷��ص���ɫ
				col += color(r, world, 0);
			}
			col /= float(ns);
			// gammar ����
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			// д��ͼ������ data[y*width*channels + x*channels + index]
			data[(ny - j - 1)*nx*3 + 3 * i + 0] = int(255.99*col[0]);
			data[(ny - j - 1)*nx*3 + 3 * i + 1] = int(255.99*col[1]);
			data[(ny - j - 1)*nx*3 + 3 * i + 2] = int(255.99*col[2]);
		}
		// print��Ⱦ����
		std::cout << (ny - j) / float(ny) * 100.0f << "%\n";
	}
	end_time = clock();
	std::cout << "Total time : " << (double)(end_time - start_time) / CLOCKS_PER_SEC << "s" << std::endl;
	// д��pngͼƬ
	stbi_write_png("book2-ch10.png", nx, ny, channels, data, 0);

	std::cout << "Completed.\n";
	system("PAUSE");
	return 0;
}