# AquaWatch - Water Quality Monitoring Application

## Team Members

- Muhammad Hadi Khan - 201758576
- Alexander McCall - 201731410
- Zhenshuo Wu - 201757147
- Alex George - 201727511
- Naomi Josebashvili - 201698359

## Overview

AquaWatch is a Qt-based desktop application designed to visualize and analyze UK/EU water quality data, with a focus on pollutant monitoring and regulatory compliance. The application provides intuitive data visualization tools and compliance tracking for environmental researchers, policymakers, and the general public. Featuring:

- **Interactive Dashboard**: Overview of key pollutant categories and compliance status
- **Pollutants Overview**: Detailed trend analysis and visualization of common pollutants
- **POPs Monitoring**: Tracking of Persistent Organic Pollutants with compliance indicators
- **Fluorinated Compounds**: Geographic visualization of PFAS and related compounds
- **Litter Indicators**: Analysis of environmental litter and debris levels
- **Compliance Dashboard**: Comprehensive regulatory compliance tracking
- **Data View**: Raw dataset exploration capabilities

## Prerequisites

- Qt 6.x
- C++17 compatible compiler
- CMake 3.16 or higher

### Required Qt Modules

- QtWidgets
- QtCharts
- QtQuickWidgets

## Building the Application

1. **Extract the Code**:

- Unzip the provided code package to a working directory.

2. Create build directory and compile:

```bash
mkdir build
cd build
cmake ..
make
```

3. Run the application:

```bash
./water
```

## Project Structure

### Core Components

- `src/`: Source code files
  - `model.cpp/h`: Data model implementation
  - `dataset.cpp/h`: Dataset handling
  - `qualitysample.cpp/h`: Sample data structure
  - Main page implementations:
    - `dashboardpage.cpp/h`
    - `trendsoverview.cpp/h`
    - `popspage.cpp/h`
    - `litterpage.cpp/h`
    - `fluorinatedcompounds.cpp/h`
    - `compliancedashboard.cpp/h`

### Additional Resources

- `styles/`: CSS stylesheets for UI components
- `dataset/`: Contains water quality data files
- Documentation:
  - Participant feedback collection
  - Consent forms
  - Requirements documentation

## Key Features

### Data Visualization

- Interactive charts and graphs
- Real-time trend analysis
- Compliance threshold indicators
- Geographic distribution visualization

### User Interface

- Intuitive navigation system
- Responsive layout design
- Consistent styling across pages
- Informative tooltips and status indicators

### Data Analysis

- Multiple pollutant category support
- Time-based trend analysis
- Geographic hotspot identification
- Compliance status tracking

## Known Issues

1. Some interface elements may not scale properly at extreme window sizes
2. Compliance threshold mismatch across pages.

## Future Improvements

- Enhanced data filtering capabilities
- Additional language support
- Advanced export functionality
- Improved map visualization features
- Real-time data update capabilities

## Testing and Feedback

The application has undergone user testing with feedback collected through:

- Heuristic evaluations
- Task-based walkthroughs
- User feedback sessions

## Contact

For questions or support, please contact: **[Hadi Khan]** - [sc23mhk@leeds.ac.uk]

## Acknowledgments

- University of Leeds COMP2811 UI module team
- Qt framework documentation and community
- Testing participants who provided valuable feedback

This README provides a comprehensive overview of the AquaWatch application while maintaining a professional and organized structure. Would you like me to expand on any particular section or add additional details?
