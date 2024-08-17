#pragma once

#include "tinyxml2.h"

#include <array>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>

/**
 * @brief Represents a 2D point (x, y).
 */
using point_type = std::array<double, 2>;

/**
 * @brief Represents a 3D color (r, g, b).
 */
using color_type = std::array<double, 3>;

/**
 * @brief Represents a 3D color at a certain parameter location (r, g, b, t).
 */
using color_point_type = std::array<double, 4>;

/**
 * @brief Types of boundary conditions.
 */
enum class boundary_condition
{
    /**
     * @brief Neumann boundary condition.
     */
    Neumann,
    /**
     * @brief Dirichlet boundary condition.
     */
    Dirichlet
};

/**
 * @brief Diffusion curve.
 */
struct diffusion_curve
{
    /**
     * @brief Coordinates of the control points.
     */
    std::vector<point_type> control_points;
    /**
     * @brief Colors on the left side.
     */
    std::vector<color_point_type> colors_left;
    /**
     * @brief Colors on the right side.
     */
    std::vector<color_point_type> colors_right;
    /**
     * @brief Boundary condition on the left.
     */
    boundary_condition boundary_left;
    /**
     * @brief Boundary condition on the right.
     */
    boundary_condition boundary_right;
};

/**
 * @brief Poisson curve.
 */
struct poisson_curve
{
    /**
     * @brief Coordinates of the control points.
     */
    std::vector<point_type> control_points;
    /**
     * @brief Laplacian at given parameter locations.
     */
    std::vector<color_point_type> weights;
};

/**
 * @brief Gradient mesh.
 */
struct gradient_mesh
{
    /**
     * @brief Number of rows of control points.
     */
    int num_rows;
    /**
     * @brief Number of columns of control points.
     */
    int num_cols;
    /**
     * @brief Linear list of control point positions.
     */
    std::vector<point_type> positions;
    /**
     * @brief Linear list of colors per control point.
     */
    std::vector<color_type> colors;
    /**
     * @brief Linear list of U tangent per control point.
     */
    std::vector<point_type> tangents_u;
    /**
     * @brief Linear list of V tangent per control point.
     */
    std::vector<point_type> tangents_v;
};

/**
 * @brief Describes a vector graphics scene.
 */
class scene
{
public:
    /**
     * @brief Read XML file with diffusion curves, Poisson curves, and gradient meshes.
     * @param _path Path to the file to read.
     * @return Scene object that contains the diffusion curves, Poisson curves, and gradient meshes.
     */
    scene(const char* _path)
        : width(0)
        , height(0)
    {
        // read the doctype
        std::ifstream infile(_path);
        if (!infile.good())
        {
            throw std::runtime_error("Cannot load XML file: " + std::string(_path));
        }
        std::string doc_type;
        std::getline(infile, doc_type);
        infile.close();

        // read xml file
        tinyxml2::XMLDocument doc;
        if (doc.LoadFile(_path) != tinyxml2::XML_SUCCESS)
        {
            throw std::runtime_error("Cannot load XML file: " + std::string(_path));
        }

        // unified scene reader
        if (doc_type == "<!DOCTYPE SceneXML>")
        {
            const tinyxml2::XMLElement* root_element = doc.FirstChildElement("scene");
            if (root_element == nullptr)
            {
                throw std::runtime_error("Cannot find scene in XML file");
            }

            root_element->QueryIntAttribute("image_width", &width);
            root_element->QueryIntAttribute("image_height", &height);

            const tinyxml2::XMLElement* diffusion_curves_element = root_element->FirstChildElement("curve_set");
            if (diffusion_curves_element != nullptr)
            {
                read_diffusion_curves(diffusion_curves_element, false);
            }

            const tinyxml2::XMLElement* poisson_curves_element = root_element->FirstChildElement("poisson_curve_set");
            if (poisson_curves_element != nullptr)
            {
                read_poisson_curves(poisson_curves_element);
            }

            const tinyxml2::XMLElement* gradient_meshes_element = root_element->FirstChildElement("mesh_set");
            if (gradient_meshes_element != nullptr)
            {
                read_gradient_meshes(gradient_meshes_element);
            }
        }
        // Orzan reader
        else if (doc_type == "<!DOCTYPE CurveSetXML>")
        {
            const tinyxml2::XMLElement* root_element = doc.RootElement();
            if (root_element == nullptr)
            {
                throw std::runtime_error("Cannot find scene in XML file");
            }

            root_element->QueryIntAttribute("image_width", &width);
            root_element->QueryIntAttribute("image_height", &height);

            read_diffusion_curves(root_element, true);
        }
        else
        {
            throw std::runtime_error("Unrecognized DOCTYPE in XML");
        }
    }

    /**
     * @brief Set of diffusion curves.
     */
    std::vector<diffusion_curve> diffusion_curves;
    /**
     * @brief Set of Poisson curves.
     */
    std::vector<poisson_curve> poisson_curves;
    /**
     * @brief Set of gradient meshes.
     */
    std::vector<gradient_mesh> gradient_meshes;
    /**
     * @brief Height of the image to render.
     */
    int height;
    /**
     * @brief Width of the image to render.
     */
    int width;

private:
    /**
     * @brief Reads diffusion curves from a given XML element.
     * @param _parent_element Parent element to read from.
     * @param _swap Swaps the x,y coordinates and the red and blue color channel. This is for compatibility with Orzan's file format.
     */
    void read_diffusion_curves(const tinyxml2::XMLElement* _parent_element, bool _swap)
    {
        int num_curves = 0;
        _parent_element->QueryIntAttribute("nb_curves", &num_curves);
        const tinyxml2::XMLElement* curve_element = _parent_element->FirstChildElement("curve");
        for (int i = 0; i < num_curves; i++)
        {
            if (curve_element == nullptr)
            {
                throw std::runtime_error("Cannot read curve " + std::to_string(i));
            }

            // -------------------- Read control points
            int num_control_points = 0;
            curve_element->QueryIntAttribute("nb_control_points", &num_control_points);
            const tinyxml2::XMLElement* point_set_spec_element = curve_element->FirstChildElement("control_points_set");
            if (point_set_spec_element == nullptr)
            {
                throw std::runtime_error("Cannot read control points of diffusion curve " + std::to_string(i));
            }

            std::vector<point_type> control_points;
            read_points(control_points, point_set_spec_element, "control_point", num_control_points, false, _swap);

            // -------------------- Read left colors
            int num_colors_left = 0;
            curve_element->QueryIntAttribute("nb_left_colors", &num_colors_left);
            const tinyxml2::XMLElement* left_color_set_spec_element = curve_element->FirstChildElement("left_colors_set");
            if (left_color_set_spec_element == nullptr)
            {
                throw std::runtime_error("Cannot read left colors of diffusion curve " + std::to_string(i));
            }

            // by default, the boundary type is Dirichlet
            boundary_condition boundary_left = boundary_condition::Dirichlet;
            if (left_color_set_spec_element->Attribute("boundary") != nullptr)
            {
                std::string boundary = left_color_set_spec_element->Attribute("boundary");
                if (boundary == "Neumann")
                    boundary_left = boundary_condition::Neumann;
            }

            // read color control points
            std::vector<color_point_type> colors_left;
            read_color_points(colors_left, left_color_set_spec_element, "left_color", num_colors_left, _swap);

            // -------------------- Read right colors
            int num_colors_right = 0;
            curve_element->QueryIntAttribute("nb_right_colors", &num_colors_right);
            const tinyxml2::XMLElement* right_color_spec_element = curve_element->FirstChildElement("right_colors_set");
            if (right_color_spec_element == nullptr)
            {
                throw std::runtime_error("Cannot read right colors of diffusion curve " + std::to_string(i));
            }

            // by default, the boundary type is Dirichlet
            boundary_condition boundary_right = boundary_condition::Dirichlet;
            if (right_color_spec_element->Attribute("boundary") != nullptr)
            {
                std::string boundary = right_color_spec_element->Attribute("boundary");
                if (boundary == "Neumann")
                    boundary_right = boundary_condition::Neumann;
            }

            // read color control points
            std::vector<color_point_type> colors_right;
            read_color_points(colors_right, right_color_spec_element, "right_color", num_colors_right, _swap);

            // -------------------- Set diffusion curve
            diffusion_curve curve;
            curve.control_points = control_points;
            curve.colors_left    = _swap ? colors_right : colors_left;
            curve.colors_right   = _swap ? colors_left : colors_right;
            curve.boundary_left  = _swap ? boundary_right : boundary_left;
            curve.boundary_right = _swap ? boundary_left : boundary_right;
            this->diffusion_curves.push_back(curve);
            curve_element = curve_element->NextSiblingElement("curve");
        }
    }

    /**
     * @brief Reads Poisson curves from a given XML element.
     * @param _parent_element Parent element to read from.
     */
    void read_poisson_curves(const tinyxml2::XMLElement* _parent_element)
    {
        int num_curves = 0;
        _parent_element->QueryIntAttribute("nb_curves", &num_curves);
        const tinyxml2::XMLElement* curve_element = _parent_element->FirstChildElement("poisson_curve");
        for (int i = 0; i < num_curves; i++)
        {
            if (curve_element == nullptr)
            {
                throw std::runtime_error("Cannot read curve " + std::to_string(i));
            }

            int num_control_points = 0;
            curve_element->QueryIntAttribute("nb_control_points", &num_control_points);
            const tinyxml2::XMLElement* point_set_spec_element = curve_element->FirstChildElement("control_points_set");
            if (point_set_spec_element == nullptr)
            {
                throw std::runtime_error("Cannot read control points of diffusion curve " + std::to_string(i));
            }

            // -------------------- Read control points
            const tinyxml2::XMLElement* point = point_set_spec_element->FirstChildElement("control_point");
            std::vector<point_type> control_points;
            read_points(control_points, point_set_spec_element, "control_point", num_control_points, false, false);

            // -------------------- Read Poisson weights
            int num_weights = 0;
            curve_element->QueryIntAttribute("nb_weights", &num_weights);
            const tinyxml2::XMLElement* weight_set_spec_element = curve_element->FirstChildElement("weights_set");
            if (weight_set_spec_element == nullptr)
            {
                throw std::runtime_error("Cannot read weights of Poisson curve " + std::to_string(i));
            }

            std::vector<color_point_type> weights;
            read_color_points(weights, weight_set_spec_element, "weight", num_weights, false);

            // -------------------- Set Poisson curve
            poisson_curve curve;
            curve.control_points = control_points;
            curve.weights        = weights;
            this->poisson_curves.push_back(curve);
            curve_element = curve_element->NextSiblingElement("poisson_curve");
        }
    }

    /**
     * @brief Reads gradient meshes from a given XML element.
     * @param _parent_element Parent element to read from.
     */
    void read_gradient_meshes(const tinyxml2::XMLElement* _parent_element)
    {
        int num_meshes = 0;
        _parent_element->QueryIntAttribute("nb_meshes", &num_meshes);
        const tinyxml2::XMLElement* mesh_spec_element = _parent_element->FirstChildElement("mesh");
        for (int i = 0; i < num_meshes; i++)
        {
            if (mesh_spec_element == nullptr)
            {
                throw std::runtime_error("Cannot read mesh " + std::to_string(i));
            }

            int num_rows = 0, num_cols = 0;
            mesh_spec_element->QueryIntAttribute("nb_rows", &num_rows);
            mesh_spec_element->QueryIntAttribute("nb_cols", &num_cols);

            // if the mesh is normalized, its positions are scaled by the image size
            bool is_normalized = false;
            mesh_spec_element->QueryBoolAttribute("normalized", &is_normalized);

            // -------------------- Read positions of the mesh vertices
            int num_positions = 0;
            mesh_spec_element->QueryIntAttribute("nb_positions", &num_positions);
            if (num_positions != (num_rows + 1) * (num_cols + 1))
            {
                throw std::runtime_error("Number of positions does not match the mesh size in mesh " + std::to_string(i));
            }

            const tinyxml2::XMLElement* vertex_set_spec_element = mesh_spec_element->FirstChildElement("position_set");
            if (vertex_set_spec_element == nullptr)
            {
                throw std::runtime_error("Cannot read positions of mesh " + std::to_string(i));
            }

            const tinyxml2::XMLElement* position = vertex_set_spec_element->FirstChildElement("position");
            std::vector<point_type> positions;
            read_points(positions, vertex_set_spec_element, "position", num_positions, is_normalized, false);

            // -------------------- Read colors of the mesh vertices
            int num_colors = 0;
            mesh_spec_element->QueryIntAttribute("nb_colors", &num_colors);
            if (num_colors != (num_rows + 1) * (num_cols + 1))
            {
                throw std::runtime_error("Number of colors does not match the mesh size in mesh " + std::to_string(i));
            }

            const tinyxml2::XMLElement* color_set_spec_element = mesh_spec_element->FirstChildElement("color_set");
            if (color_set_spec_element == nullptr)
            {
                throw std::runtime_error("Cannot read colors of mesh " + std::to_string(i));
            }

            std::vector<color_type> colors;
            read_colors(colors, color_set_spec_element, "color", num_colors, false);

            // Read optional position tangents
            std::vector<point_type> tangents_u;
            std::vector<point_type> tangents_v;
            const tinyxml2::XMLElement* tangent_set_spec_element = mesh_spec_element->FirstChildElement("pos_tangent_set");
            if (tangent_set_spec_element != nullptr)
            {
                read_points(tangents_u, tangent_set_spec_element, "positionU", num_positions, is_normalized, false);
                read_points(tangents_v, tangent_set_spec_element, "positionV", num_positions, is_normalized, false);
            }

            // -------------------- Set gradient mesh
            gradient_mesh mesh;
            mesh.num_rows   = num_rows;
            mesh.num_cols   = num_cols;
            mesh.positions  = positions;
            mesh.colors     = colors;
            mesh.tangents_u = tangents_u;
            mesh.tangents_v = tangents_v;
            this->gradient_meshes.push_back(mesh);

            mesh_spec_element = mesh_spec_element->NextSiblingElement("mesh");
        }
    }

    /**
     * @brief Read 2D positions (x,y) from the XML file.
     * @param _points Output vector that receives the points.
     * @param _parent Parent XML element to read from.
     * @param _child_name Name of the child to read from.
     * @param _num_points Number of points to read.
     * @param _is_normalized Flag that determines whether the data comes in normalized.
     * @param _swap Flag that enables swapping of the x,y coordinates.
     */
    void read_points(std::vector<point_type>& _points, const tinyxml2::XMLElement* _parent, const std::string& _child_name, int _num_points, bool _is_normalized, bool _swap)
    {
        const tinyxml2::XMLElement* child_element = _parent->FirstChildElement(_child_name.c_str());
        if (child_element == nullptr)
        {
            throw std::runtime_error("Cannot read " + _child_name);
        }

        for (int i = 0; i < _num_points; i++)
        {
            if (child_element == nullptr)
            {
                throw std::runtime_error("Cannot read " + _child_name + " " + std::to_string(i));
            }

            point_type p;
            child_element->QueryDoubleAttribute("x", &p[0]);
            child_element->QueryDoubleAttribute("y", &p[1]);

            if (_is_normalized)
            {
                p[0] *= this->height;
                p[1] *= this->width;
            }

            if (_swap)
            {
                // swap x and y components
                std::swap(p[0], p[1]);
            }

            _points.push_back(p);
            child_element = child_element->NextSiblingElement(_child_name.c_str());
        }
    }

    /**
     * @brief Read 3D colors (R,G,B) from the XML file
     * @param _colors Output vector that receives the colors.
     * @param _parent Parent XML element to read from.
     * @param _child_name Name of the child to read from.
     * @param _num_colors Number of colors to read.
     * @param _swap Flag that enables swapping of the R and B channel.
     */
    void read_colors(std::vector<color_type>& _colors, const tinyxml2::XMLElement* _parent, const std::string& _child_name, int _num_colors, bool _swap)
    {
        const tinyxml2::XMLElement* child_element = _parent->FirstChildElement(_child_name.c_str());
        if (child_element == nullptr)
        {
            throw std::runtime_error("Cannot read " + _child_name);
        }

        for (int i = 0; i < _num_colors; i++)
        {
            if (child_element == nullptr)
            {
                throw std::runtime_error("Cannot read " + _child_name + " " + std::to_string(i));
            }

            color_type c;
            if (child_element->Attribute("R") != nullptr)
            {
                child_element->QueryDoubleAttribute("R", &c[0]);
                c[0] /= 255.0;
            }
            else
            {
                child_element->QueryDoubleAttribute("r", &c[0]);
            }

            if (child_element->Attribute("G") != nullptr)
            {
                child_element->QueryDoubleAttribute("G", &c[1]);
                c[1] /= 255.0;
            }
            else
            {
                child_element->QueryDoubleAttribute("g", &c[1]);
            }

            if (child_element->Attribute("B") != nullptr)
            {
                child_element->QueryDoubleAttribute("B", &c[2]);
                c[2] /= 255.0;
            }
            else
            {
                child_element->QueryDoubleAttribute("b", &c[2]);
            }

            if (_swap)
            {
                // swap R and B components
                std::swap(c[0], c[2]);
            }

            _colors.push_back(c);
            child_element = child_element->NextSiblingElement(_child_name.c_str());
        }
    }

    /**
     * @brief Read 3D colors (r,g,b) with parameter location (t) from the XML file
     * @param _color_points Output vector that receives the colors and parameter location.
     * @param _parent Parent XML element to read from.
     * @param _child_name Name of the child to read from.
     * @param _num_points Number of points to read.
     * @param _swap Flag that enables swapping of the R and B channel.
     */
    void read_color_points(std::vector<color_point_type>& _color_points, const tinyxml2::XMLElement* _parent, const std::string& _child_name, int _num_points, bool _swap)
    {
        const tinyxml2::XMLElement* child_element = _parent->FirstChildElement(_child_name.c_str());
        if (child_element == nullptr)
        {
            throw std::runtime_error("Cannot read " + _child_name);
        }

        double maxT = -std::numeric_limits<double>::max();
        for (int i = 0; i < _num_points; i++)
        {
            if (child_element == nullptr)
            {
                throw std::runtime_error("Cannot read " + _child_name + " " + std::to_string(i));
            }

            color_point_type cp;
            if (child_element->Attribute("R") != nullptr)
            {
                child_element->QueryDoubleAttribute("R", &cp[0]);
                cp[0] /= 255.0;
            }
            else
            {
                child_element->QueryDoubleAttribute("r", &cp[0]);
            }

            if (child_element->Attribute("G") != nullptr)
            {
                child_element->QueryDoubleAttribute("G", &cp[1]);
                cp[1] /= 255.0;
            }
            else
            {
                child_element->QueryDoubleAttribute("g", &cp[1]);
            }

            if (child_element->Attribute("B") != nullptr)
            {
                child_element->QueryDoubleAttribute("B", &cp[2]);
                cp[2] /= 255.0;
            }
            else
            {
                child_element->QueryDoubleAttribute("b", &cp[2]);
            }

            if (_swap)
            {
                // swap R and B components
                std::swap(cp[0], cp[2]);
            }

            child_element->QueryDoubleAttribute("globalID", &cp[3]);
            if (cp[3] > 1)
            {
                maxT = std::max(maxT, cp[3]);
            }

            _color_points.push_back(cp);
            child_element = child_element->NextSiblingElement(_child_name.c_str());
        }

        // Normalize the globalID
        if (maxT > 1)
        {
            for (color_point_type& cp : _color_points)
            {
                cp[3] /= maxT;
            }
        }
    }
};
