#include "Object.h"

std::map<Ball, Object::Data> Object::data;

bool Object::data_exists(Ball ball)
{
	return !data[ball].vertices.empty();
}

void Object::extract_data(std::string_view path_obj)
{
	tinyobj::attrib_t attrib;
	std::vector <tinyobj::shape_t> shapes;
	std::vector <tinyobj::material_t> materials;
	std::string warn, err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path_obj.data());
	if (!warn.empty()) { std::cout << warn << "\n"; }
	if (!err.empty()) { throw std::exception(err.c_str()); }
	if (!ret) { exit(1); }

	for (size_t s = 0; s < shapes.size(); s++)
	{
		size_t index_offset = 0;

		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
		{
			size_t fv = shapes[s].mesh.num_face_vertices[f];

			for (size_t v = 0; v < fv; v++)
			{
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

				float vx = attrib.vertices[3 * idx.vertex_index + 0];
				float vy = attrib.vertices[3 * idx.vertex_index + 1];
				float vz = attrib.vertices[3 * idx.vertex_index + 2];

				data[ball].vertices.push_back(vx);
				data[ball].vertices.push_back(vy);
				data[ball].vertices.push_back(vz);

				if (!attrib.normals.empty()) {
					float nx = attrib.normals[3 * idx.normal_index + 0];
					float ny = attrib.normals[3 * idx.normal_index + 1];
					float nz = attrib.normals[3 * idx.normal_index + 2];

					data[ball].normals.push_back(nx);
					data[ball].normals.push_back(ny);
					data[ball].normals.push_back(nz);
				}

				if (!attrib.texcoords.empty()) {
					float tx = attrib.texcoords[2 * idx.texcoord_index + 0];
					float ty = attrib.texcoords[2 * idx.texcoord_index + 1];

					data[ball].tex_coords.push_back(tx);
					data[ball].tex_coords.push_back(-ty);
				}

				if (!attrib.colors.empty()) {
					float red = attrib.colors[3 * idx.vertex_index + 0];
					float green = attrib.colors[3 * idx.vertex_index + 1];
					float blue = attrib.colors[3 * idx.vertex_index + 2];

					data[ball].colors.push_back(red);
					data[ball].colors.push_back(green);
					data[ball].colors.push_back(blue);
				}
				data[ball].indices.push_back(v + index_offset);
			}
			index_offset += fv;
		}
	}
	float r = 0.0f, g = 0.0f, b = 0.0f;
	switch (ball) {
	case Ball::Red:
		r = 1.0f; g = 0.0f; b = 0.0f;
		break;
	case Ball::Green:
		r = 0.0f; g = 1.0f; b = 0.0f;
		break;
	case Ball::Blue:
		r = 0.0f; g = 0.0f; b = 1.0f;
		break;
	case Ball::Pink:
		r = 1.0f; g = 0.42f; b = 0.7f;
		break;
	case Ball::Yellow:
		r = 1.0f; g = 1.0f; b = 0.0f;
		break;
	case Ball::Orange:
		r = 1.0f; g = 0.6f; b = 0.0f;
		break;
	default:
		break;
	}

	for (int i = 0; i < data[ball].colors.size(); i += 3)
	{
		data[ball].colors[i] = r;
		data[ball].colors[i + 1] = g;
		data[ball].colors[i + 2] = b;
	}
}

void Object::apply_texture(std::string_view path_texture)
{
	if (destroyed) { return; }
	int width, height, comp;
	stbi_uc* pixels = stbi_load(path_texture.data(), &width, &height, &comp, 4);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	this->tex_loc = glGetUniformLocation(shader, "tex1");
	glUniform1i(this->tex_loc, 0);
}

void Object::init_fbo()
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &fboTex);
	glBindTexture(GL_TEXTURE_2D, fboTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Object::init_vbo()
{
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, data[ball].vertices.size() * sizeof(data[ball].vertices[0]),
		data[ball].vertices.data(), GL_STATIC_DRAW);

	this->pos_loc = glGetAttribLocation(shader, "pos");
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(this->pos_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(this->pos_loc);
}

void Object::init_cbo()
{
	glGenBuffers(1, &cbo);
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glBufferData(GL_ARRAY_BUFFER, data[ball].colors.size() * sizeof(data[ball].colors[0]),
		data[ball].colors.data(), GL_STATIC_DRAW);

	this->color_loc = glGetAttribLocation(shader, "color");
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glVertexAttribPointer(this->color_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(this->color_loc);
}

void Object::init_nbo()
{
	glGenBuffers(1, &nbo);
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glBufferData(GL_ARRAY_BUFFER, data[ball].normals.size() * sizeof(data[ball].normals[0]),
		data[ball].normals.data(), GL_STATIC_DRAW);

	this->norm_loc = glGetAttribLocation(shader, "normal");
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glVertexAttribPointer(this->norm_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(this->norm_loc);
}

void Object::init_tbo()
{
	glGenBuffers(1, &tbo);
	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	glBufferData(GL_ARRAY_BUFFER, data[ball].tex_coords.size() * sizeof(data[ball].tex_coords[0]),
		data[ball].tex_coords.data(), GL_STATIC_DRAW);

	this->tc_loc = glGetAttribLocation(shader, "tcoord");
	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	glVertexAttribPointer(this->tc_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(this->tc_loc);
}

void Object::init_ibo()
{
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, data[ball].indices.size() * sizeof(data[ball].indices[0]),
		data[ball].indices.data(), GL_STATIC_DRAW);
}

void Object::init_transform()
{
	this->transform = glm::mat4(1);
	this->transf_loc = glGetUniformLocation(shader, "transform");

	this->norm_transform = glm::mat4(1);
	this->norm_transf_loc = glGetUniformLocation(shader, "rtransform");
}

Object::Object(Ball ball, std::string_view path_obj, std::string_view path_texture, Program& shader, XYZPoint position) :
	shader(shader), ball(ball)
{
	if (!this->data_exists(ball)) { extract_data(path_obj); }

	init_vbo();
	init_cbo();
	init_nbo();
	init_tbo();
	init_ibo();
	//init_fbo();

	init_transform();

	apply_texture(path_texture);

	move_to(position);
}

Object::Object(std::vector<float> vertices, std::vector<float> colors, std::vector<float> normals, std::vector<unsigned int> indices, Program& shader) :
	shader(shader), ball(Ball::Nothing)
{
	data[Ball::Nothing].vertices = vertices;
	data[Ball::Nothing].colors = colors;
	data[Ball::Nothing].normals = normals;
	data[Ball::Nothing].indices = indices;

	init_vbo();
	init_cbo();
	init_nbo();
	init_tbo();
	init_ibo();

	init_transform();
}

//Object::Object(const Object& other)
//{
//	shader = other.shader;
//	ball = other.ball;
//
//	init_vbo();
//	init_cbo();
//	init_nbo();
//	init_tbo();
//	init_ibo();
//
//	transform = other.transform;
//	transf_loc = other.transf_loc;
//
//	norm_transform = other.norm_transform;
//	norm_transf_loc = other.norm_transf_loc;
//
//	projection = other.projection;
//	proj_loc = other.proj_loc;
//
//	//apply_texture(otherpath_texture);
//}

void Object::draw()
{
	if (destroyed) { return; }

	glUseProgram(shader);

	glUniformMatrix4fv(this->transf_loc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniformMatrix4fv(this->norm_transf_loc, 1, GL_FALSE, glm::value_ptr(norm_transform));

	this->pos_loc = glGetAttribLocation(shader, "pos");
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(this->pos_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(this->pos_loc);

	this->norm_loc = glGetAttribLocation(shader, "normal");
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glVertexAttribPointer(this->norm_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(this->norm_loc);

	this->color_loc = glGetAttribLocation(shader, "color");
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glVertexAttribPointer(this->color_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(this->color_loc);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	glDrawElements(GL_TRIANGLES, data[ball].indices.size(), GL_UNSIGNED_INT, nullptr);
}

void Object::set_projection(glm::vec3 cameraPos, glm::vec3 lookAt, float fov, float aspect, float nearClip, float farClip)
{
	if (destroyed) { return; }
	this->projection = glm::mat4(1);
	this->projection = glm::perspective(fov, aspect, nearClip, farClip);
	this->projection *= (glm::lookAt(cameraPos, lookAt, glm::vec3(0, 1, 0)));

	this->proj_loc = glGetUniformLocation(shader, "projection");
	glUniformMatrix4fv(this->proj_loc, 1, GL_FALSE, glm::value_ptr(this->projection));
}

void Object::move_to(XYZPoint point)
{
	if (destroyed) { return; }
	init_transform();

	transform = glm::translate(transform, point);
	norm_transform = glm::transpose(glm::inverse(transform));

	glUniformMatrix4fv(this->transf_loc, 1, GL_FALSE, glm::value_ptr<float>(transform));
	glUniformMatrix4fv(this->norm_transf_loc, 1, GL_FALSE, glm::value_ptr<float>(norm_transform));
}

void Object::reinit(Ball ball, XYZPoint position)
{
	this->ball = ball;
	destroyed = false;

	init_vbo();
	init_cbo();
	init_nbo();
	init_tbo();
	init_ibo();

	move_to(position);
}

void Object::move_to(Matrix4 transform, Matrix4 norm_transform)
{
	if (destroyed) { return; }
	this->transform = transform;
	this->norm_transform = norm_transform;

	glUniformMatrix4fv(this->transf_loc, 1, GL_FALSE, glm::value_ptr<float>(transform));
	glUniformMatrix4fv(this->norm_transf_loc, 1, GL_FALSE, glm::value_ptr<float>(norm_transform));
}

void Object::rotate(float angle)
{
	if (destroyed) { return; }
	transform = glm::rotate(transform, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f)); // 0, 0, 1

	glUniformMatrix4fv(this->transf_loc, 1, GL_FALSE, glm::value_ptr<float>(transform));
	glUniformMatrix4fv(this->norm_transf_loc, 1, GL_FALSE, glm::value_ptr<float>(norm_transform));
}

Object::~Object()
{
	glDeleteRenderbuffers(1, &rbo);
	glDeleteFramebuffers(1, &fbo);
	glDisableVertexAttribArray(pos_loc);
	glDisableVertexAttribArray(color_loc);
	glDisableVertexAttribArray(norm_loc);
	glDisableVertexAttribArray(tc_loc);
	glDisableVertexAttribArray(tex_loc);
}
