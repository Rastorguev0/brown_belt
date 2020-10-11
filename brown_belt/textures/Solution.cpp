#include "Common.h"
#include "Textures.h"

using namespace std;

// Этот файл сдаётся на проверку
// Здесь напишите реализацию необходимых классов-потомков `IShape`

class Ellipse : public IShape {
public:
	std::unique_ptr<IShape> Clone() const {
		auto shape = *this;
		return make_unique<Ellipse>(shape);
	}

	void SetPosition(Point point) {
		position_ = point;
	}
	Point GetPosition() const {
		return position_;
	}

	void SetSize(Size size) {
		size_ = size;
	}
	Size GetSize() const {
		return size_;
	}

	void SetTexture(std::shared_ptr<ITexture> texture) {
		texture_ = texture;
	}
	ITexture* GetTexture() const {
		return texture_.get();
	}

	// Рисует фигуру на указанном изображении
	void Draw(Image& image) const {
		Size size = {};
		Image texture_image;
		if (texture_) {
			texture_image = texture_->GetImage();
			size = texture_->GetSize();
		}
		for (int y = position_.y; y < size_.height + position_.y; ++y) {
			for (int x = position_.x; x < size_.width + position_.x; ++x) {
				if (y < image.size() && x < image[y].size()) {
					if (IsPointInEllipse({ x - position_.x, y - position_.y }, size_)) {
						if (y - position_.y < size.height
							&& x - position_.x < size.width) {
							image[y][x] = texture_image[y - position_.y][x - position_.x];
						}
						else image[y][x] = '.';
					}
				}
			}
		}
	}
private:
	Point position_ = {};
	Size size_ = {};
	shared_ptr<ITexture> texture_ = nullptr;
};

class Rectangle : public IShape {
public:
	std::unique_ptr<IShape> Clone() const {
		auto shape = *this;
		return make_unique<Rectangle>(shape);
	}

	void SetPosition(Point point) {
		position_ = point;
	}
	Point GetPosition() const {
		return position_;
	}

	void SetSize(Size size) {
		size_ = size;
	}
	Size GetSize() const {
		return size_;
	}

	void SetTexture(std::shared_ptr<ITexture> texture) {
		texture_ = texture;
	}
	ITexture* GetTexture() const {
		return texture_.get();
	}

	// Рисует фигуру на указанном изображении
	void Draw(Image& image) const {
		Size size = {};
		Image texture_image;
		if (texture_) {
			texture_image = texture_->GetImage();
			size = texture_->GetSize();
		}
		for (int y = position_.y; y < size_.height + position_.y; ++y) {
			for (int x = position_.x; x < size_.width + position_.x; ++x) {
				if (y < image.size() && x < image[y].size()) {
					if (y - position_.y < size.height
						&& x - position_.x < size.width) {
						image[y][x] = texture_image[y - position_.y][x - position_.x];
					}
					else image[y][x] = '.';
				}
			}
		}
	}
private:
	Point position_;
	Size size_ = {};
	shared_ptr<ITexture> texture_;
};

// Напишите реализацию функции
unique_ptr<IShape> MakeShape(ShapeType shape_type) {
	switch (shape_type) {
	case ShapeType::Rectangle: return make_unique<Rectangle>();
	case ShapeType::Ellipse: return make_unique<Ellipse>();
	}
	return 0;
}