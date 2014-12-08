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

        if(current_line_)
        {
            Line cur = current_line_.get();
            drawer.set_color(Qt::green);
            drawer.draw_line(cur.getSegment().first, cur.getSegment().second);
        }


//        drawer.set_color(Qt::green);
//        for(int i=0; i<old_points_.size(); ++i)
//        {
//            drawer.draw_point(old_points_[i], 10);
//        }

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
            //vector<pair<point_2, point_2> > v;
            //dcel_->getAllToDraw(old_points_, v);
            delete dcel_;
            dcel_ = new LinkedTriangleDcel(l1, l2, l3);
            return true;
        }

        if(key_code == Qt::Key_I)
        {
            dcel_ = new LinkedInfiniteDcel();
            //old_points_.resize(0);
            lines_.resize(0);
            return true;
        }
        return false;
    }


    Dcel* dcel_;
    //LinkedTriangleDcel triangle_dcel_;
private:
    std::vector<Line> lines_;

    boost::optional<point_2f> current_point_;
    boost::optional<point_2f> normal_point_;
    boost::optional<Line> current_line_;

    //vector<point_2> old_points_;

};

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
    sample_viewer viewer;
    cg::visualization::run_viewer(&viewer, "lines displayer");
}
