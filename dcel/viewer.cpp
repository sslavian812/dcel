#include <vector>
#include <algorithm>

#include <QColor>
#include <QApplication>

#include <boost/optional.hpp>
#include <cg/visualization/viewer_adapter.h>
#include <cg/primitives/point.h>

#include "line.h"
//#include "orientation.h"
#include "dcel.h"


using cg::point_2f;
using std::pair;

struct sample_viewer : cg::visualization::viewer_adapter
{

   void drawFace(cg::visualization::drawer_type & drawer, int face) const
   {
       if(dcel_.faces.size()==1)
           return;
       if(dcel_.faces.size()==2)
       {
            int e1=0;

            drawer.set_color(Qt::yellow);
            pair<point_2, point_2> edge = dcel_.getEdge(e1);
            drawer.draw_line(edge.first, edge.second);
            drawer.set_color(Qt::red);
            drawer.draw_point(edge.first, 5);
            drawer.draw_point(edge.second, 5);
            return;
       }

       int start_edge = dcel_.faces[face].startEdge;
       int cur_edge = start_edge;
       pair<point_2, point_2> edge;
       do
       {
           pair<point_2, point_2> edge = dcel_.getEdge(cur_edge);

           drawer.set_color(Qt::blue);
           drawer.draw_line(edge.first, edge.second);
           drawer.set_color(Qt::red);
           drawer.draw_point(edge.first, 5);
           drawer.draw_point(edge.second, 5);
           cur_edge = dcel_.edges[cur_edge].next;
       }while(cur_edge != start_edge);
   }

   void draw(cg::visualization::drawer_type & drawer) const
   {
      int face = 0;
      int n_faces=dcel_.faces.size();
      for(int i=0; i<n_faces; ++i)
          drawFace(drawer, i);


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
      if (!current_point_)
         return false;

      if (!normal_point_)
         return false;

      Line l(current_point_.get(), normal_point_.get()-current_point_.get()); // Line(point_2f, vector_2f);

      lines_.push_back(l);
      //sort(lines_.begin(), lines_.end());

      current_line_.reset();

      dcel_.addLine(l.a, l.b, l.c);

      current_point_.reset();
      normal_point_.reset();


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

private:
   Dcel dcel_;
   std::vector<Line> lines_;
   boost::optional<point_2f> current_point_;
   boost::optional<point_2f> normal_point_;
   boost::optional<Line> current_line_;
   //boost::optional<Line> first_;
   //boost::optional<Line> second_;
};

int main(int argc, char ** argv)
{
   QApplication app(argc, argv);
   sample_viewer viewer;
   cg::visualization::run_viewer(&viewer, "lines displayer");
}
