#include <vector>

#include <QColor>
#include <QApplication>

#include <boost/optional.hpp>
#include <cg/visualization/viewer_adapter.h>
#include <cg/primitives/point.h>

#include "line.h"



using cg::point_2f;


struct sample_viewer : cg::visualization::viewer_adapter
{
   void draw(cg::visualization::drawer_type & drawer) const
   {
      drawer.set_color(Qt::white);
      for (int i = 0; i < lines_.size(); ++i)
      {
          Line l = lines_[i];
          drawer.draw_line(l.getSegment().first, l.getSegment().second);
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
      if (!current_point_)
         return false;
      if (!normal_point_)
         return false;

      Line l(current_point_.get(), normal_point_.get() - current_point_.get()); // Line(point_2f, vector_2f);
      lines_.push_back(l);


      current_point_.reset();
      normal_point_.reset();
      return true;
   }

   bool on_move(const point_2f & p)
   {
       if (!current_point_)
          return false;

       if(!normal_point_ || normal_point_.get() != current_point_.get())
           normal_point_ = p;
       else
           return false;

       current_line_ = Line(current_point_.get(), normal_point_.get() - current_point_.get());
       return true;
    }

private:
   std::vector<Line> lines_;
   boost::optional<point_2f> current_point_;
   boost::optional<point_2f> normal_point_;
   boost::optional<Line> current_line_;
};

int main(int argc, char ** argv)
{
   QApplication app(argc, argv);
   sample_viewer viewer;
   cg::visualization::run_viewer(&viewer, "lines displayer");
}
