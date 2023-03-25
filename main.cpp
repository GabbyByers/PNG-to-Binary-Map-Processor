#include <iostream>
#include <string>
#include <vector>
#include "SFML/Graphics.hpp"

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

struct vertex
{
	int i = -1;
	int j = -1;
	vertex* prev = nullptr;
	vertex* next = nullptr;
};

class polygon
{
public:
	vector<pixel*> pixels;
	vector<vertex> unsorted_vertices;
	vector<vertex> sorted_vertices;

	polygon() {}

	void sort_vertices()
	{
		assign_prev_and_next();
	}

	void assign_prev_and_next() // TODO: implement
	{
		for (int i = 0; i < unsorted_vertices.size(); i++)
		{
			vertex* vertex = &unsorted_vertices[i];

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
	int current_index = 0;

	pixel_queue() {}

	void push(pixel* pixel)
	{
		pixels.push_back(pixel);
	}

	pixel* pop()
	{
		if (current_index >= pixels.size())
		{
			return nullptr;
		}
		return pixels[current_index];
		current_index++;
	}

	bool empty()
	{
		return current_index >= pixels.size();
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
	}

	void populate_provinces()
	{
		traversable_image& image = *image_ptr;
		for (int i = 0; i < image.width; i++)
		{
			for (int j = 0; j < image.height; j++)
			{
				pixel* pixel = &image.head[i][j];

				// continue if color is white
				if (pixel->color == sf::Color::White)
				{
					continue;
				}

				// continue if pixel has been visited
				if (pixel->visited)
				{
					continue;
				}

				// make a new province if a province of that color does not already exist
				if (novel_color(&pixel->color))
				{
					province new_province;
					new_province.color = pixel->color;
					provinces.push_back(new_province);
				}

				// get province for whom we will now give a new polygon
				province* this_province = &provinces[0]; // default to avoid null pointer
				for (int i = 0; i < provinces.size(); i++)
				{
					if (provinces[i].color == pixel->color)
					{
						this_province = &provinces[i];
						break;
					}
				}

				polygon new_polygon;
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

	void flood(polygon* poly, pixel* origin_pixel)
	{
		traversable_image& image = *image_ptr;
		
		pixel_queue queue;
		queue.push(origin_pixel);

		while (true)
		{
			if (queue.empty())
			{
				break;
			}

			pixel* popped_pixel = queue.pop();
			poly->pixels.push_back(popped_pixel);
			
			// get each of the four cardinal neighbours
			pixel* neighbours[4]
			{
				&image.head[popped_pixel->i + 1][popped_pixel->j],
				&image.head[popped_pixel->i - 1][popped_pixel->j],
				&image.head[popped_pixel->i][popped_pixel->j + 1],
				&image.head[popped_pixel->i][popped_pixel->j - 1]
			};

			// for each of those four pixels, add them to the queue if they have not been visited and if they are the right color
			for (pixel* neighbour : neighbours)
			{
				if (neighbour->color != origin_pixel->color)
				{
					continue;
				}
				if (neighbour->visited)
				{
					continue;
				}
				neighbour->visited = true;
				queue.push(neighbour);
			}
		}
	}
};

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	sf::RenderWindow window(sf::VideoMode(500, 500), "Hello SFML", sf::Style::Close);
	sf::Event event;
	
	world world("japan.png");

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		window.clear(sf::Color::Black);
		window.display();
	}

	return 0;
}