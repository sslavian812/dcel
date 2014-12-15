#include <vector>
#include <algorithm>

#include <QColor>
#include <QApplication>

#include <boost/optional.hpp>
#include <cg/visualization/viewer_adapter.h>
#include <cg/primitives/point.h>

#include "line.h"
#include "dcel.h"
#include "linked_infinite_dcel.h"
#include "linked_triangle_dcel.h"


using cg::point_2f;
using std::pair;
using std::vector;

struct sample_viewer : cg::visualization::viewer_adapter
{
    sample_viewer()
    {
        dcel_ = new LinkedInfiniteDcel();
    }

    void draw(cg::visualization::drawer_type & drawer) const
    {

        vector<point_2> res_vertices;
        vector<pair<point_2, point_2> > res_edges;

        dcel_->getAllToDraw(res_vertices, res_edges);

        drawer.set_color(Qt::red);

        for(int i=0; i<res_vertices.size(); ++i)
        {
            drawer.draw_point(res_vertices[i], 5);
        }

        drawer.set_color(Qt::blue);

        for(int i=0; i<res_edges.size(); ++i)
        {
            drawer.draw_line(res_edges[i].first, res_edges[i].second);
        }

        drawer.set_color(Qt::white);
        for(int i=0; i<actual_face_.size(); ++i)
        {
            drawer.draw_line(actual_face_[i].first, actual_face_[i].second, 3);
        }


        if(current_line_)
        {
            Line cur = current_line_.get();
            drawer.set_color(Qt::green);
            drawer.draw_line(cur.getSegment().first, cur.getSegment().second);
        }
    }

    void print(cg::visualization::printer_type & p) const
    {
        p.corner_stream() << "press lbutton to fix a point on line" << cg::visualization::endl
                        << "move mouse to specify line's normal vector" << cg::visualization::endl
                        << "lines count: " <<lines_.size() << cg::visualization::endl;
    }


    bool on_press(const point_2f & p)
    {
        if (current_point_)
            return false;

        current_point_ = p;
        return true;
    }

    bool on_release(const point_2f & p)
    {
        if(localization_mode_)
        {
            dcel_->localize(p, actual_face_);
            current_point_.reset();
            normal_point_.reset();
            current_line_.reset();
            return true;
        }

        if (!current_point_)
            return false;

        if (!normal_point_)
            return false;

        Line l(current_point_.get(), normal_point_.get()-current_point_.get()); // Line(point_2f, vector_2f);
        current_line_.reset();

        current_point_.reset();
        normal_point_.reset();

        if(find(lines_.begin(), lines_.end(), l) != lines_.end())
            return false;

        dcel_->addLine(l.a, l.b, l.c);
        lines_.push_back(l);
        return true;
    }

    bool on_move(const point_2f & p)
    {
        if (!current_point_)
            return false;

        if(p != current_point_.get())
            normal_point_ = p;
        else
            return false;

        current_line_ = Line(current_point_.get(), normal_point_.get() - current_point_.get());
        return true;
    }


    bool on_key(int key_code)
    {
        if(key_code == Qt::Key_T)
        {
            Line l1,l2,l3;
            dcel_->getBounds(l1, l2, l3);
            delete dcel_;
            dcel_ = new LinkedTriangleDcel(l1, l2, l3);

            for(int i=0; i<lines_.size(); ++i)
            {
                Line l = lines_[i];
                if(l == l1 || l == l2 || l == l3)
                    continue;
                dcel_->addLine(l.a, l.b, l.c);
            }
            lines_.resize(0);

            return true;
        }

        if(key_code == Qt::Key_I)
        {
            dcel_ = new LinkedInfiniteDcel();
            lines_.resize(0);
            return true;
        }
        if(key_code == Qt::Key_L)
        {
            localization_mode_ = true;
            return true;
        }
        if(key_code == Qt::Key_A)
        {
            localization_mode_ = false;
            actual_face_ = {};
        }
        return false;
    }


    Dcel* dcel_;
private:
    std::vector<Line> lines_;

    boost::optional<point_2f> current_point_;
    boost::optional<point_2f> normal_point_;
    boost::optional<Line> current_line_;

    std::vector<pair<point_2, point_2> > actual_face_; // vector of segments

    bool localization_mode_ = false;
    //vector<point_2> old_points_;

};

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
    sample_viewer viewer;
    cg::visualization::run_viewer(&viewer, "lines displayer");
}
