#include <iostream>
#include <string>
#include <vector>
#include "SFML/Graphics.hpp"
#include "mouse.h"

using namespace std;
#define _CRTDBG_MAP_ALLOC

struct pixel
{
	int i = -1;
	int j = -1;
	bool visited = false;
	sf::Color color = sf::Color::White;
};

class traversable_image
{
public:
	int width = -1;
	int height = -1;
	sf::Image image;
	pixel** head = nullptr;

	traversable_image(string filename)
	{
		load_image(filename);
		allocate_pixels();
		initialize_pixel_values();
	}

	void load_image(string filename)
	{
		image.loadFromFile(filename);
		width = image.getSize().x;
		height = image.getSize().y;
	}

	void allocate_pixels()
	{
		head = new pixel*[width];
		for (int i = 0; i < width; i++)
		{
			head[i] = new pixel[height];
		}
	}

	void initialize_pixel_values()
	{
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				pixel* pixel = &head[i][j];
				pixel->i = i;
				pixel->j = j;
				pixel->color = image.getPixel(i, j);
			}
		}
	}

	~traversable_image()
	{
		for (int i = 0; i < width; i++)
		{
			delete[] head[i];
		}
		delete[] head;
	}
};

class vertex
{
public:
	int i = 0;
	int j = 0;
	vertex* prev = nullptr;
	vertex* next = nullptr;
	char variant = '\0';

	vertex(int _i, int _j)
	{
		i = _i;
		j = _j;
	}
};

class polygon
{
public:
	traversable_image* image_ptr = nullptr;
	vector<pixel*> pixels;
	vector<vertex> vertices;

	polygon(traversable_image* _image_ptr)
	{
		image_ptr = _image_ptr;
	}

	void initialize_vertices()
	{
		traversable_image& image = *image_ptr;

		for (pixel* this_pixel : pixels)
		{
			int i = 2 * this_pixel->i;
			int j = 2 * this_pixel->j;

			pixel* t_pixel = &image.head[this_pixel->i][this_pixel->j - 1];
			pixel* b_pixel = &image.head[this_pixel->i][this_pixel->j + 1];
			pixel* l_pixel = &image.head[this_pixel->i - 1][this_pixel->j];
			pixel* r_pixel = &image.head[this_pixel->i + 1][this_pixel->j];

			sf::Color& center = this_pixel->color;
			sf::Color& top    = t_pixel->color;
			sf::Color& bottom = b_pixel->color;
			sf::Color& left   = l_pixel->color;
			sf::Color& right  = r_pixel->color;

			//TOP
			if (center != top)
			{
				vertex new_vertex(i + 1, j);
				new_vertex.variant = 'H';
				vertices.push_back(new_vertex);
			}

			//BOTTOM
			if (center != bottom)
			{
				vertex new_vertex(i + 1, j + 2);
				new_vertex.variant = 'H';
				vertices.push_back(new_vertex);
			}

			//LEFT
			if (center != left)
			{
				vertex new_vertex(i, j + 1);
				new_vertex.variant = 'V';
				vertices.push_back(new_vertex);
			}

			//RIGHT
			if (center != right)
			{
				vertex new_vertex(i + 2, j + 1);
				new_vertex.variant = 'V';
				vertices.push_back(new_vertex);
			}

			//TOP LEFT
			if (center != top && center != left)
			{
				if (top != left)
				{
					vertex new_vertex(i, j);
					new_vertex.variant = 'C';
					vertices.push_back(new_vertex);
				}
			}

			//TOP RIGHT
			if (center != top && center != right)
			{
				if (top != right)
				{
					vertex new_vertex(i + 2, j);
					new_vertex.variant = 'C';
					vertices.push_back(new_vertex);
				}
			}

			//BOTTOM LEFT
			if (center != bottom && center != left)
			{
				if (bottom != left)
				{
					vertex new_vertex(i, j + 2);
					new_vertex.variant = 'C';
					vertices.push_back(new_vertex);
				}
			}

			//BOTTOM RIGHT
			if (center != bottom && center != right)
			{
				if (bottom != right)
				{
					vertex new_vertex(i + 2, j + 2);
					new_vertex.variant = 'C';
					vertices.push_back(new_vertex);
				}
			}
		}
	}
};

struct province
{
	sf::Color color;
	vector<polygon> polygons;
};

class pixel_queue
{
public:
	vector<pixel*> pixels;

	pixel_queue() {}

	void push(pixel* pixel)
	{
		pixels.push_back(pixel);
	}

	pixel* pop()
	{
		pixel* pixel = pixels[0];
		pixels.erase(pixels.begin());
		return pixel;
	}

	bool empty()
	{
		return pixels.empty();
	}
};

class world
{
public:
	vector <province> provinces;
	traversable_image* image_ptr = nullptr;

	world(string filename)
	{
		image_ptr = new traversable_image(filename);
		populate_provinces();
		process_polygons();
	}

	void populate_provinces()
	{
		traversable_image& image = *image_ptr;
		for (int i = 0; i < image.width; i++)
		{
			for (int j = 0; j < image.height; j++)
			{
				pixel* pixel = &image.head[i][j];

				// mark the pixel as visited if the color is white
				if (pixel->color == sf::Color::White)
				{
					pixel->visited = true;
				}

				// continue if pixel has been visited
				if (pixel->visited)
				{
					continue;
				}

				// having passed the first two basic tests, mark the pixel as visited
				pixel->visited = true;

				// make a new province if a province of that color does not already exist
				if (novel_color(&pixel->color))
				{
					province new_province;
					new_province.color = pixel->color;
					provinces.push_back(new_province);
				}

				// get a pointer to the province of the correct color
				province* this_province = &provinces[0]; // default to avoid null pointer
				for (int i = 0; i < provinces.size(); i++)
				{
					if (provinces[i].color == pixel->color)
					{
						this_province = &provinces[i];
						break;
					}
				}

				polygon new_polygon(image_ptr);
				flood(&new_polygon, pixel);
				this_province->polygons.push_back(new_polygon);
			}
		}
	}

	bool novel_color(sf::Color* color)
	{
		for (int i = 0; i < provinces.size(); i++)
		{
			province* province = &provinces[i];
			if (province->color == *color)
			{
				return false;
			}
		}
		return true;
	}

	void flood(polygon* poly, pixel* origional_pixel)
	{	
		traversable_image& image = *image_ptr;
		
		pixel_queue queue;
		queue.push(origional_pixel);
		
		while (true)
		{
			if (queue.empty())
			{
				break;
			}

			pixel* popped_pixel = queue.pop();
			poly->pixels.push_back(popped_pixel);

			int i = popped_pixel->i;
			int j = popped_pixel->j;
			pixel* neighbours[4]
			{
				&image.head[i + 1][j],
				&image.head[i - 1][j],
				&image.head[i][j + 1],
				&image.head[i][j - 1]
			};

			for (int i = 0; i < 4; i++)
			{
				pixel* neighbour = neighbours[i];
				if (neighbour->visited)
				{
					continue;
				}
				if (neighbour->color != origional_pixel->color)
				{
					continue;
				}
				neighbour->visited = true;
				queue.push(neighbour);
			}
		}
	}

	void process_polygons()
	{
		for (province& prov : provinces)
		{
			for (polygon& poly : prov.polygons)
			{
				poly.initialize_vertices();
			}
		}
	}
};

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	int screen_width = 1500;
	int screen_height = 800;

	sf::RenderWindow window(sf::VideoMode(screen_width, screen_height), "Hello SFML", sf::Style::Close);
	mouse mouse(screen_width, screen_height);
	sf::Event event;
	
	world world("images/japan.png");

	vector<sf::Vertex> squares;
	float x_offset = 0;
	float y_offset = 0;
	float zoom = 0.1;
	float scale = 1;
	float small_scale = 0.2;
	float margin = 0.05;

	while (window.isOpen())
	{
		mouse.set_mouse_properties(sf::Mouse::getPosition(window));
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			if (event.type == sf::Event::MouseWheelMoved)
			{
				scale *= 1 + (event.mouseWheel.delta * zoom);
				x_offset += (mouse.x - x_offset) * (-1 * zoom * event.mouseWheel.delta);
				y_offset += (mouse.y - y_offset) * (-1 * zoom * event.mouseWheel.delta);
			}
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		{
			x_offset += mouse.rel_x;
			y_offset += mouse.rel_y;
		}

		squares.clear();

		// pixels
		for (province& province : world.provinces)
		{
			for (polygon& polygon : province.polygons)
			{
				for (pixel* p_pixel : polygon.pixels)
				{
					pixel& pixel = *p_pixel;
					squares.push_back(sf::Vertex(sf::Vector2f(scale * pixel.i + x_offset,                            scale * pixel.j + y_offset), pixel.color));
					squares.push_back(sf::Vertex(sf::Vector2f(scale * pixel.i + x_offset + (scale - scale * margin), scale * pixel.j + y_offset), pixel.color));
					squares.push_back(sf::Vertex(sf::Vector2f(scale * pixel.i + x_offset + (scale - scale * margin), scale * pixel.j + y_offset + (scale - scale * margin)), pixel.color));
					squares.push_back(sf::Vertex(sf::Vector2f(scale * pixel.i + x_offset,                            scale * pixel.j + y_offset + (scale - scale * margin)), pixel.color));
				}
			}
		}

		// vertices
		sf::Color vertex_color;
		for (province& province : world.provinces)
		{
			for (polygon& polygon : province.polygons)
			{
				for (vertex& vertex : polygon.vertices)
				{
					float i = vertex.i / 2.0f;
					float j = vertex.j / 2.0f;
					if (vertex.variant == 'H') { vertex_color = sf::Color(255, 0, 0); }
					if (vertex.variant == 'V') { vertex_color = sf::Color(0, 255, 0); }
					if (vertex.variant == 'C') { vertex_color = sf::Color(255, 255, 255); }
					squares.push_back(sf::Vertex(sf::Vector2f(scale * i + x_offset - (0.5f * (scale * small_scale)),                         scale * j - (0.5f * (scale * small_scale)) + y_offset), vertex_color));
					squares.push_back(sf::Vertex(sf::Vector2f(scale * i + x_offset - (0.5f * (scale * small_scale)) + (scale * small_scale), scale * j - (0.5f * (scale * small_scale)) + y_offset), vertex_color));
					squares.push_back(sf::Vertex(sf::Vector2f(scale * i + x_offset - (0.5f * (scale * small_scale)) + (scale * small_scale), scale * j - (0.5f * (scale * small_scale)) + y_offset + (scale * small_scale)), vertex_color));
					squares.push_back(sf::Vertex(sf::Vector2f(scale * i + x_offset - (0.5f * (scale * small_scale)),                         scale * j - (0.5f * (scale * small_scale)) + y_offset + (scale * small_scale)), vertex_color));
				}
			}
		}

		window.clear(sf::Color::Black);
		window.draw(&squares[0], squares.size(), sf::Quads);
		window.display();
	}

	return 0;
}