/*
	Main Application Window implementation
*/

#include <QtWidgets>

#include "MainWindow.h"
#include "Samplers.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(Volume& volume, QWidget *parent) :
	QMainWindow(parent),
	m_volume(std::move(volume)),
	m_equalizer(&m_volume)
{
	setWindowTitle("title");
	setFixedSize(QSize(1280, 720));
	setCentralWidget(createWidgets());
}

MainWindow::~MainWindow()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::writeSubimage(QImage& target, const VolumeSubimage& view)
{
	auto h = target.height();
	auto w = target.width();


	for (size_t j = 0; j < target.height(); j++)
	{
		for (size_t i = 0; i < target.width(); i++)
		{
			//quint8 col = view.volume()->equalize(view.at(i,j));

			const auto u = (float)i / target.width();
			const auto v = (float)j / target.height();
			
			const quint8 col = m_equalizer.convert(BasicSampler::sample(view, u, v));
			target.setPixel(i, j, qRgb(col, col, col));
		}
	}
}

void MainWindow::setImageSide(int value)
{
	QImage target(256, 256, QImage::Format_Grayscale8);
	//writeSubimage(target, m_volume.getZ(value));
	writeSubimage(target, VolumeSubimage(&m_volume, value, VolumeAxis::XAxis));
	m_sideImage->setPixmap(QPixmap::fromImage(target));
}

void MainWindow::setImageFront(int value)
{
	QImage target(256, 256, QImage::Format_Grayscale8);
	//writeSubimage(target, m_volume.getX(value));
	writeSubimage(target, VolumeSubimage(&m_volume, value, VolumeAxis::YAxis));
	m_frontImage->setPixmap(QPixmap::fromImage(target));
}

void MainWindow::setImageTop(int value)
{
	QImage target(256, 256, QImage::Format_Grayscale8);
	//writeSubimage(target, m_volume.getY(value));
	writeSubimage(target, VolumeSubimage(&m_volume, value, VolumeAxis::ZAxis));
	m_topImage->setPixmap(QPixmap::fromImage(target));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

QWidget* MainWindow::createWidgets()
{
	//Central widget layout
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(createControlArea());
	layout->addWidget(createImageArea());

	//Central widget
	QWidget* center = new QWidget(this);
	center->setLayout(layout);

	//Slider event
	connect(m_xSlider, &QSlider::valueChanged, this, &MainWindow::setImageFront);
	connect(m_ySlider, &QSlider::valueChanged, this, &MainWindow::setImageTop);
	connect(m_zSlider, &QSlider::valueChanged, this, &MainWindow::setImageSide);

	m_xSlider->setSliderPosition(76);
	m_ySlider->setSliderPosition(76);
	m_zSlider->setSliderPosition(76);

	return center;
}

QWidget* MainWindow::createImageArea()
{
	//Images
	m_topImage = new QLabel(this);
	m_topImage->setAlignment(Qt::AlignCenter);
	m_sideImage = new QLabel(this);
	m_sideImage->setAlignment(Qt::AlignCenter);
	m_frontImage = new QLabel(this);
	m_frontImage->setAlignment(Qt::AlignCenter);

	//Image grid
	QGridLayout* imageLayout = new QGridLayout(this);
	imageLayout->addWidget(m_topImage, 0, 0);
	imageLayout->addWidget(m_sideImage, 0, 1);
	imageLayout->addWidget(m_frontImage, 1, 0);

	QScrollArea* scrollArea = new QScrollArea(this);
	scrollArea->setLayout(imageLayout);
	return scrollArea;
}

QWidget* MainWindow::createControlArea()
{
	QFormLayout* ctrlLayout = new QFormLayout(this);
	ctrlLayout->setMargin(5);
	ctrlLayout->setAlignment(Qt::AlignTop);
	
	m_xSlider = new QSlider(Qt::Horizontal, this);
	m_xSlider->setRange(0, m_volume.rows() - 1);
	ctrlLayout->addRow(QStringLiteral("Front"), m_xSlider);

	m_ySlider = new QSlider(Qt::Horizontal, this);
	m_ySlider->setRange(0, m_volume.slices() - 1);
	ctrlLayout->addRow(QStringLiteral("Top"), m_ySlider);

	m_zSlider = new QSlider(Qt::Horizontal, this);
	m_zSlider->setRange(0, m_volume.columns() - 1);
	ctrlLayout->addRow(QStringLiteral("Side"), m_zSlider);

	QGroupBox* ctrlGroup = new QGroupBox(QStringLiteral("Options"), this);
	ctrlGroup->setLayout(ctrlLayout);
	ctrlGroup->setMaximumWidth(300);
	ctrlGroup->setMinimumWidth(200);

	return ctrlGroup;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
